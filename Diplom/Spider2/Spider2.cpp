#include <iostream>
#include <string>
#include <regex>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/asio.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <vector>
#include <sstream>
#include <algorithm>
#include <pqxx/pqxx>
#include <Windows.h>
#include <future>
#pragma execution_character_set ( "utf-8" )

namespace beast = boost::beast;        // из Boost.Beast
namespace http = beast::http;          // из Boost.Beast.HTTP
namespace net = boost::asio;           // из Boost.Asio
using tcp = boost::asio::ip::tcp;      // из Boost.Asio
namespace ssl = net::ssl;

int const max_redirect_count = 5;

std::string to_lowercase(const std::string& input) {
    std::string output = input; // Копируем входную строку
    std::transform(output.begin(), output.end(), output.begin(),
        [](unsigned char c) { return std::tolower(c); }); // Преобразуем в нижний регистр
    return output;
}

std::vector<std::pair<std::string, int>> my_word_index(std::string clean_response) {
    std::string lower_response = to_lowercase(clean_response);
    std::vector<std::pair<std::string, int>> index_vector;
    std::istringstream f(lower_response);
    std::string str;
    std::pair<std::string, int> temp_pair;
    temp_pair.first = "";
    temp_pair.second = 0;
    bool check_coincidence = false;
    
    while (std::getline(f, str, ' ')) {  
        for (auto &elem : index_vector) {
            if (str == elem.first) {                
                ++elem.second;
                check_coincidence = true;
                break;
            }            
        }    
        if(!check_coincidence) {
        temp_pair.first = str;
        temp_pair.second = 1;
        index_vector.push_back(temp_pair);        
        }
        check_coincidence = false;
    }
    //for (auto const& elem : index_vector) {
      //  std::cout << elem.first << " " << elem.second << std::endl;
    //}

    return index_vector;
}

std::vector<std::string> extract_links(const std::string& text) {
    std::vector<std::string> links;
    // Регулярное выражение для поиска URL
    std::regex url_regex(R"((https?://[^\s]+))");
    std::smatch url_match;

    // Ищем все совпадения в тексте
    auto search_start = text.cbegin();
    while (std::regex_search(search_start, text.cend(), url_match, url_regex)) {
        links.push_back(url_match[0]); // Добавляем найденный URL в вектор
        search_start = url_match.suffix().first; // Продолжаем поиск после найденного URL
    }

    return links;
}

std::string clean_html(const std::string& input) {
    // Удаляем HTML-теги
    std::string output = std::regex_replace(input, std::regex("<[^>]*>"), " ");

    // Удаляем знаки препинания
    output = std::regex_replace(output, std::regex("[^\\w\\s]"), " ");

    // Заменяем переносы строк и табуляцию на пробелы
    output = std::regex_replace(output, std::regex("[\\n\\r\\t]+"), " ");

    // Удаляем лишние пробелы
    output = std::regex_replace(output, std::regex("\\s+"), " ");

    // Удаляем пробелы в начале и конце строки
    output = std::regex_replace(output, std::regex("^\\s+|\\s+$"), "");

    return output;
}

void my_index(std::string &url, std::vector<std::pair<std::string,int>> &index_vec, pqxx::connection &con) {
    pqxx::work ac{ con };
    std::string word;
    int inx = 1;
    for (auto const& elem : index_vec) {
        if (elem.first.size() < 16 && url.size() < 501) {
            ac.exec("INSERT INTO index_base (url, word, count)"
                "VALUES ('" + ac.esc(url) + "', '" + ac.esc(elem.first) + "', '" + std::to_string(elem.second) + "' );");
        }
    }

    ac.commit();
}


std::future<int> fetchPage(std::string& url, pqxx::connection& con, int current_depth, int depth, int redirect_count) {
    return std::async(std::launch::async, [url, &con, current_depth, depth, redirect_count]() mutable {
        int result = EXIT_SUCCESS;
        try {

            // Создаем ввод-вывод контекст и TCP сокет
            net::io_context io_context;
            ssl::context ssl_context(ssl::context::sslv23);
            ssl_context.set_default_verify_paths(); // Установка путей для проверки сертификатов        

            // Разделяем URL на хост и путь
            std::string host;
            std::string path;
            auto pos = url.find("://");
            if (pos != std::string::npos) {
                pos += 3; // Пропускаем "://"
                auto path_pos = url.find('/', pos);
                host = url.substr(pos, path_pos - pos);
                path = path_pos != std::string::npos ? url.substr(path_pos) : "/";
            }
            else {
                host = url;
                path = "/";
            }

            // Создаем сокет и резолвер
            tcp::resolver resolver(io_context);
            //beast::tcp_stream stream(io_context);
            beast::ssl_stream<beast::tcp_stream> stream(io_context, ssl_context);

            // Получаем конечные точки
            auto const endpoints = resolver.resolve(host, "https");
            //net::connect(stream.socket(), endpoints);
            net::connect(stream.next_layer().socket(), endpoints);

            // Устанавливаем SSL соединение
            stream.handshake(ssl::stream_base::client);

            // Создаем HTTP-запрос
            http::request<http::string_body> req{ http::verb::get, path, 11 };
            req.set(http::field::host, host);
            req.set(http::field::user_agent, "Boost.Beast/248");

            // Отправляем запрос
            http::write(stream, req);

            // Получаем ответ
            beast::flat_buffer buffer; // Буфер для хранения ответа
            http::response<http::string_body> res;
            http::read(stream, buffer, res);

            // Проверяем код ответа
            std::cout << "Response code: " << res.result_int() << std::endl;
            std::cout << "Current depth: " << current_depth << std::endl;

            if (res.result_int() != 404) {

                // Обработка перенаправлений
                if (redirect_count < max_redirect_count) {
                    if (res.result() == http::status::moved_permanently ||
                        res.result() == http::status::found) {
                        ++redirect_count;

                        std::cout << redirect_count << std::endl;

                        // Извлекаем новый URL из заголовка Location
                        auto location = std::string(res["Location"].data(), res["Location"].size());
                        std::cout << "Redirecting to: " << location << std::endl;
                        url = location; // Обновляем URL для следующего запроса
                        if (current_depth < depth) {
                            result = fetchPage(url, con, current_depth, depth, redirect_count).get(); // Повторяем запрос с новым URL
                        }
                    }
                }

                // Если не перенаправление, выводим ответ и выходим

                std::string clean = clean_html(res.body());

                ++current_depth;

                std::vector<std::pair<std::string, int>> indx_vec = my_word_index(clean);
                std::vector<std::string> url_vec = extract_links(res.body());
                                
                my_index(url, indx_vec, con);
                if (current_depth < depth) {
                    std::vector<std::future<int>> futures;
                    for (auto const& elem : url_vec) {
                        std::string temp_url_str = elem;
                        url = temp_url_str;
                        futures.push_back(fetchPage(temp_url_str, con, current_depth, depth, redirect_count)); // Повторяем запрос с новым URL                    
                    }
                    for (auto& fut : futures) {
                        fut.get(); // Получаем результат
                    }
                }

            }

            // Закрываем сокет
            beast::error_code ec;
            stream.shutdown(ec);
        }

        catch (std::exception& e) {
            std::cerr << "Ошибка: " << e.what() << std::endl;
            return EXIT_FAILURE;
        }

        return EXIT_SUCCESS;   
     });
}




int main() {

    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
    setvbuf(stdout, nullptr, _IOFBF, 1000);

    int current_depth = 0;
    int redirect_count = 0;

    try {
        boost::property_tree::ptree pt;
        boost::property_tree::ini_parser::read_ini("../../../Config.ini", pt);

        std::string host = pt.get<std::string>("DataBase.host");
        std::cout << "host: " << host << std::endl;

        std::string port = pt.get<std::string>("DataBase.port");
        std::cout << "port: " << port << std::endl;

        std::string db_name = pt.get<std::string>("DataBase.db_name");
        std::cout << "dbname: " << db_name << std::endl;

        std::string user = pt.get<std::string>("DataBase.user");
        std::cout << "user: " << user << std::endl;

        std::string password = pt.get<std::string>("DataBase.password");
        std::cout << "password: " << password << std::endl;

        std::string start_page = pt.get<std::string>("Spider.StartPage");
        std::cout << "Start page: " << start_page << std::endl;

        std::string str_depth = pt.get<std::string>("Spider.Depth");
        std::cout << "Depth: " << str_depth << std::endl;

        int int_depth = stoi(str_depth);        
 
        pqxx::connection con(
            "host=" + host +
            " port=" + port +
            " dbname=" + db_name +
            " user=" + user +
            " password=" + password);
       

    static const char* bs_start =
        "CREATE TABLE IF NOT EXISTS index_base(        \
             id SERIAL PRIMARY KEY,                         \
             url varchar(500) NOT NULL,                    \
             word varchar(15) NOT NULL,               \
             count int NOT NULL            \
             ); ";

    pqxx::transaction tr{ con };

    tr.exec(bs_start);

    tr.commit();

    auto future_result = fetchPage(start_page, con, current_depth, int_depth, redirect_count);

    int result = future_result.get();

}
catch (const boost::property_tree::ini_parser_error& e) {
    std::cerr << "INI Parser Error: " << e.what() << std::endl;
}
catch (const std::exception& e)
{
    std::cout << "Exception: " << e.what() << std::endl;
}
catch (...) {
    std::cerr << "Unknown error occurred." << std::endl;
}


    return 0;
}