#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <iostream>
#include <string>
#include <regex>
#include <pqxx/pqxx>

namespace beast = boost::beast; // ��� ��������
namespace http = beast::http;    // ��� HTTP
namespace net = boost::asio;      // ��� ����
using tcp = boost::asio::ip::tcp; // ��� TCP

std::string clean_html(const std::string& input) {
    // ������� HTML-����
    std::string output = std::regex_replace(input, std::regex("<[^>]*>"), " ");

    // ������� ����� ����������
    output = std::regex_replace(output, std::regex("[^\\w\\s]"), " ");

    // �������� �������� ����� � ��������� �� �������
    output = std::regex_replace(output, std::regex("[\\n\\r\\t]+"), " ");

    // ������� ������ �������
    output = std::regex_replace(output, std::regex("\\s+"), " ");

    // ������� ������� � ������ � ����� ������
    output = std::regex_replace(output, std::regex("^\\s+|\\s+$"), "");

    return output;
}

std::string to_lowercase(const std::string& input) {
    std::string output = input; // �������� ������� ������
    std::transform(output.begin(), output.end(), output.begin(),
        [](unsigned char c) { return std::tolower(c); }); // ����������� � ������ �������
    return output;
}


// ������� ��� ��������� HTML-�������� � ������ ������
std::string generateHtmlForm() {
    return R"(
        <!DOCTYPE html>
        <html>
        <head>
            <title>�����</title>
        </head>
        <body>
            <h1>����� URL</h1>
            <form action="/search" method="post">
                <input type="text" name="word" placeholder="������� ��������� ������ (�� 4� ����):" required>
                <button type="submit">�����</button>
            </form>
        </body>
        </html>
    )";
}

// ��������� POST-�������
void handlePostRequest(const http::request<http::string_body>& req, http::response<http::string_body>& res, pqxx::connection& con) {
    // �������� ������ �� �����
    std::string word = req.body();

    std::cout << word << std::endl;

    word = clean_html(word);

    word = to_lowercase(word);

    std::istringstream f(word);

    std::string str;

    std::vector<std::string> word_vec;    

    pqxx::work db_select{ con };

    bool clean_check = false;

    bool not_found = false;

    bool req_too_long = false;

    std::string responseBody;

    while (std::getline(f, str, ' ')) {
        if (clean_check) {
            word_vec.push_back(str);
        }
        clean_check = true;
    }       

    for (auto const& elem : word_vec) {
        std::cout << elem << std::endl;
    }    

    if (word_vec.size() > 4) {
        responseBody = "������� ������� ������, ����������, ������������ �������, �� �� �����, ��� ������ ����.";
        req_too_long = true;
    }

    if (!req_too_long) {

    std::pair<std::string, int> temp_pair;

    std::vector<std::vector<std::pair<std::string, int>>> handle_vec;

    std::vector<std::pair<std::string, int>> final_vec;

    int wvs = word_vec.size();

    if (!word_vec.empty()) {                      
        for (auto const& elem : word_vec) {
            {
                std::vector<std::pair<std::string, int>> temp_vec;
                for (auto [url, count] : db_select.query<std::string, int>(
                    "SELECT url, count FROM index_base WHERE word = '" + db_select.esc(elem) + "' "))
                {
                    temp_pair.first = url;
                    temp_pair.second = count;
                    temp_vec.push_back(temp_pair);
                    if (url != "") {
                        bool check = false;
                        for (auto const& elem : final_vec) {
                            if (elem.first == url) check = true;
                        }

                        if (!check) {
                            if (wvs > 1) temp_pair.second = 0;
                            final_vec.push_back(temp_pair);
                        }
                    }
                    
                }
                if (!temp_vec.empty()) handle_vec.push_back(temp_vec);
            }
        }
    }


    for (auto const& elem : final_vec) {
        std::cout << elem.first << " " << elem.second << std::endl;
    }

    std::cout << "\\\\" << std::endl;


    int it = 0;
    for (auto const& el : handle_vec) {
        for (auto const& elem : el) {
            std::cout << it << elem.first << " " << elem.second << std::endl;
        }
        ++it;
    }
    

    if (handle_vec.size() < word_vec.size()) {
        responseBody = "����� �� ������� ������ ����, �� ��� �����������";
        not_found = true;
    }    
        if (!not_found) { 
            if (!handle_vec.empty()) {
                if (handle_vec.size() > 1) {
                    for (auto const& el : handle_vec) {
                        {
                        std::vector<std::pair<std::string, int>> temp_vec;
                        for (auto const& elem : el) {                     
                                for (auto const& f_elem : final_vec) {
                                    if (elem.first == f_elem.first) {
                                        temp_pair.first = f_elem.first;
                                        temp_pair.second = f_elem.second + elem.second;
                                        temp_vec.push_back(temp_pair);
                                    }
                                }                                                            
                        }
                        final_vec = temp_vec;
                        }
                    }
                }
            }

            std::sort(final_vec.begin(), final_vec.end(), [](const auto& a, const auto& b) {
                return a.second > b.second; 
            });         

            
                        
            for (auto const& elem : final_vec) {
                responseBody = responseBody + elem.first + "\n";
                std::cout << elem.first << " " << elem.second <<  std::endl;
            }

            if (responseBody != "") {
                responseBody = "������, ��������� �� ������ ������� �������:\n" + responseBody;
            }
        }
    }

    //std::cout << responseBody;
    if (responseBody == "") {
        responseBody = "�� ������ �������, ����������� �� �������.";
    }

    res.result(http::status::ok);
    res.set(http::field::content_type, "text/plain");
    res.body() = responseBody;
    res.prepare_payload();
}

// �������� ���������� HTTP-��������
void handleRequest(const http::request<http::string_body>& req, http::response<http::string_body>& res, pqxx::connection& con) {
    if (req.method() == http::verb::get) {
        // ���������� HTML-��������
        res.result(http::status::ok);
        res.set(http::field::content_type, "text/html");
        res.body() = generateHtmlForm();
        res.prepare_payload();
    }
    else if (req.method() == http::verb::post) {
        handlePostRequest(req, res, con);
    }
    else {
        res.result(http::status::bad_request);
        res.body() = "Invalid request method.";
        res.prepare_payload();
    }
}

// ������ �������
void doSession(tcp::socket& socket, pqxx::connection& con){
    beast::flat_buffer buffer;
    http::request<http::string_body> req;

    boost::system::error_code ec;

    // ������ �������
    http::read(socket, buffer, req, ec);
    if (ec) {
        std::cerr << "Error reading request: " << ec.message() << std::endl;
        return;
    }

    // ������������ ������
    http::response<http::string_body> res{ http::status::ok, req.version() };
    handleRequest(req, res, con);

    // �������� ������
    http::write(socket, res, ec);
    if (ec) {
        std::cerr << "Error writing response: " << ec.message() << std::endl;
    }
}

// �������� �������
int main() {

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

        std::string str_port = pt.get<std::string>("Server.port");
        std::cout << "Server port: " << str_port << std::endl;

        int serv_port = stoi(str_port);

        pqxx::connection con(
            "host=" + host +
            " port=" + port +
            " dbname=" + db_name +
            " user=" + user +
            " password=" + password);

        net::io_context ioc;
        tcp::acceptor acceptor{ ioc, {tcp::v4(), static_cast<unsigned short>(serv_port)} };

        while (true) {
            tcp::socket socket{ ioc };
            acceptor.accept(socket);
            doSession(socket, con);
        }
    }
    catch (const boost::property_tree::ini_parser_error& e) {
        std::cerr << "INI Parser Error: " << e.what() << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }    
    catch (...) {
        std::cerr << "Unknown error occurred." << std::endl;
    }

    return 0;
}