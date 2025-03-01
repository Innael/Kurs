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
#include <atomic>
#include <chrono>
#include <thread>
#include <queue>
#include <mutex>
#include <functional>
#include <condition_variable>
#pragma execution_character_set ( "utf-8" )


namespace beast = boost::beast;        // из Boost.Beast
namespace http = beast::http;          // из Boost.Beast.HTTP
namespace net = boost::asio;           // из Boost.Asio
using tcp = boost::asio::ip::tcp;      // из Boost.Asio
namespace ssl = net::ssl;

int const max_redirect_count = 5;
int global_depth = 1;

std::atomic<bool> transaction_in_progress = false;


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

void extract_links(const std::string& text, const std::string& base_url, std::vector<std::pair<std::string, int>>& url_vec, int current_depth, std::vector<std::string>& check_vec) {
    std::pair<std::string, int> temp;
    temp.second = ++current_depth;

    // Находим последний слеш в базовом URL
    size_t last_slash = base_url.find_last_of('/');
    // Если базовый URL заканчивается на '/', убираем его
    std::string base = (last_slash == base_url.length() - 1) ? base_url : base_url.substr(0, last_slash);

    // Регулярное выражение для поиска тегов <a> с атрибутом href
    std::regex a_tag_regex(R"(<a\s+href=["']?([^"'>]+)["']?[^>]*>)");
    std::smatch a_tag_match;

    // Ищем все совпадения в тексте
    auto search_start = text.cbegin();
    while (std::regex_search(search_start, text.cend(), a_tag_match, a_tag_regex)) {
        std::string found_link = a_tag_match[1]; // Извлекаем значение href

        // Удаляем фрагмент, если он есть
        size_t fragment_pos = found_link.find('#');
        if (fragment_pos != std::string::npos) {
            found_link = found_link.substr(0, fragment_pos); // Обрезаем до символа '#'
        }

        // Игнорируем ссылки, начинающиеся с '#'
        if (found_link.empty() || found_link[0] == '#') {
            search_start = a_tag_match.suffix().first; // Продолжаем поиск
            continue;
        }

        // Проверяем, является ли ссылка абсолютной
        if (found_link.find("http://") != std::string::npos || found_link.find("https://") != std::string::npos) {
            temp.first = found_link;
        }
        else {
            // Если это относительная ссылка, преобразуем её в абсолютную
            if (found_link[0] == '/') {
                // Если ссылка начинается с '/', используем базовый домен
                size_t link_size = found_link.size();
                last_slash = found_link.find_last_of('/');
                found_link = found_link.substr(last_slash, link_size);
                temp.first = base + found_link;
            }
            else {
                temp.first = base + "/" + found_link;
            }
        }
        //std::cout << temp.first << std::endl;
        if (found_link.find("index.php?") != std::string::npos){                   // проверяем нет ли "мусорных" ссылок 
        }
        else {
            bool check_url = false;
            for (const auto& elem : check_vec) {
                if (elem == temp.first) {
                    check_url = true;
                    break;
                }
            }
            if (!check_url) {
                url_vec.push_back(temp); // Добавляем найденный URL в вектор 
                check_vec.push_back(temp.first);
                check_url = false;
            }
        }        

        // Сдвигаем итератор на позицию после найденного тега
        auto iterator_check = search_start;
        search_start = a_tag_match.suffix().first;
        if (iterator_check == search_start) break;
    }
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

    while (transaction_in_progress.load()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    transaction_in_progress.store(true);

    for (auto const& elem : index_vec) {
        if (elem.first.size() < 16 && url.size() < 501) {
            ac.exec("INSERT INTO index_base (url, word, count)"
                "VALUES ('" + ac.esc(url) + "', '" + ac.esc(elem.first) + "', '" + std::to_string(elem.second) + "' );");
        }        
    }

    ac.commit();

    transaction_in_progress.store(false);
}

void my_fetchPage(std::vector<std::pair<std::string, int>>& url_vec, std::pair<std::string, int> url_pair, pqxx::connection& con, std::vector<std::string>& check_vec){
    try {

        int current_depth = url_pair.second;
        int redirect_count = 0;
        std::string url = url_pair.first;
        bool success = false;


        while (success == false) {
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
            beast::ssl_stream<beast::tcp_stream> stream(io_context, ssl_context);

            if (!SSL_set_tlsext_host_name(stream.native_handle(), host.c_str()))
            {
                boost::system::error_code ec{ static_cast<int>(::ERR_get_error()), boost::asio::error::get_ssl_category() };
                throw boost::system::system_error{ ec };
            }

            // Получаем конечные точки
            auto const endpoints = resolver.resolve(host, "https");
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
            std::cout << "Url: " << url << std::endl;

            if (res.result_int() != 404) {

                // Обработка перенаправлений
                if (redirect_count < max_redirect_count) {
                    if (res.result() == http::status::moved_permanently ||
                        res.result() == http::status::found) {
                        ++redirect_count;

                        std::cout << "Redirect count:" << redirect_count << std::endl;

                        // Извлекаем новый URL из заголовка Location
                        auto location = std::string(res["Location"].data(), res["Location"].size());
                        std::cout << "Redirecting to: " << location << std::endl;
                        url = location; // Обновляем URL для следующего запроса
                        
                    }
                    else success = true;
                }
                else {
                    beast::error_code ec;
                    stream.shutdown(ec);
                    break;
                } 

                if (success == true) {                          // Если не перенаправление, выводим ответ и выходим
                    std::string clean = clean_html(res.body());                    

                    std::vector<std::pair<std::string, int>> indx_vec = my_word_index(clean);
                    if (current_depth < global_depth) {
                        extract_links(res.body(), url, url_vec, current_depth, check_vec);
                    }
                    

                    my_index(url, indx_vec, con);
                }

            }
            else {
                beast::error_code ec;
                stream.shutdown(ec);
                break;
            }
            beast::error_code ec;
            stream.shutdown(ec);  // Закрываем сокет
        }     
        
    }

    catch (std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;       
    }
}

class safe_queue {
public:
    std::queue<std::pair<std::string, int>> f_queue;
    std::mutex safe_queue_mutex;
    std::condition_variable sq_convar;
    std::atomic<bool> noty = false;
    std::vector<std::pair<std::string, int>>* vec_ptr = nullptr;
    std::vector<std::string>* url_check_ptr = nullptr;
    pqxx::connection* con_ptr = nullptr;




    void push(std::pair<std::string, int> url_pair) {
        std::lock_guard<std::mutex> lk(safe_queue_mutex);
        f_queue.push(url_pair);
        //std::cout << "Адрес помещён в очередь\n";
        noty = true;
        sq_convar.notify_one();
    }

    void pop() {
        std::pair<std::string, int> temp;
        std::unique_lock<std::mutex> ulk(safe_queue_mutex);
        sq_convar.wait(ulk, [&] { return noty == true; });
        temp = f_queue.front();
        f_queue.pop();
        my_fetchPage(*vec_ptr, temp, *con_ptr, *url_check_ptr);
        if (f_queue.empty() == true) noty = false;
    }

    void Set_url_vec(std::vector<std::pair<std::string, int>>& url_vec) {
        vec_ptr = &url_vec;
    }

    void Set_con_ptr(pqxx::connection& con) {
        con_ptr = &con;
    }

    void Set_check_vec(std::vector<std::string>& url_check_vec) {
        url_check_ptr = &url_check_vec;
    }

};


class thread_pool {
public:
    std::vector<std::thread> tr_vec;
    safe_queue tp_sq;
    int core_count = 1;
    std::atomic<int> pool = 1;
    std::atomic<bool> ready = false;
    std::atomic<bool> stop = false;

    thread_pool(std::vector<std::pair<std::string, int>>& url_vec, pqxx::connection& con, std::vector<std::string>& url_check_vec) {
        core_count = std::thread::hardware_concurrency();
        pool = core_count;
        tp_sq.Set_url_vec(url_vec);
        tp_sq.Set_con_ptr(con);
        tp_sq.Set_check_vec(url_check_vec);
        for (int i = 0; i < pool; ++i) {
            tr_vec.push_back(std::thread(&thread_pool::work, this));            
        }
        ready = true;
    }

    void work() {
        while (!ready) {
            std::this_thread::yield();
        }
        for (int i = 0; i < 2;) {
            if (tp_sq.f_queue.empty() == false) {                
                tp_sq.pop();                
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));

            if (tp_sq.f_queue.empty() == true && stop == true) i = 2;
        }
    }

    void submit(std::pair<std::string, int> url_pair) {
        tp_sq.push(url_pair);
    }


    ~thread_pool() {
        for (auto& elem : tr_vec) {
            if (elem.joinable()) {
                elem.join();
            }
        }
    }
};


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

    global_depth = int_depth;

    std::pair <std::string, int> first_pair;
    first_pair.first = start_page;
    first_pair.second = 1;

    std::vector<std::pair<std::string, int>> url_vector;
    url_vector.push_back(first_pair);
    std::vector<std::string> check_url_vec;
    check_url_vec.push_back(start_page);

    my_fetchPage(url_vector, url_vector[0], con, check_url_vec);
    
    thread_pool th_pool(url_vector, con, check_url_vec);

        for (int r = 1; r < url_vector.size(); ++r) {  
            if (url_vector[r].second < int_depth+1) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                th_pool.submit(url_vector[r]);
            }
        }
        th_pool.stop = true;
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