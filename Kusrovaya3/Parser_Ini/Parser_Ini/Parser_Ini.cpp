#include <iostream>
#include <fstream>
#include <windows.h>
#include <string>
#include <vector>


class WrongSintaksis : public std::exception
{
    int stnum = 0;
public:
    WrongSintaksis(int stnum) {
        this->stnum = stnum;
    }

    const char* what() const override { return "Синтаксическая ошибка в строке: "; }

    int Get_stnum() {
        return this->stnum;
    }
};

class NotFoundValue : public std::exception
{   
public: 
    const char* what() const override { return "Данной переменной не присвоено значение.";}    
};

class NotFoundFile : public std::exception
{
public:
    const char* what() const override { return "Не получилось загрузить нужный файл или указанное имя файла неверно."; }
};

class NotFoundVariable : public std::exception
{
public:
    const char* what() const override { return "\nУказанная вами переменная, в данной секции файла, не найдена";}
};
 



class Parser_ini {
    std::string section = "";
    std::string value = "";
    std::string fname = "";
    std::ifstream file;

public:
    Parser_ini(std::string filename) {
        this->fname = filename;
        this->file.open(fname);
        if (file.is_open() == false) {
            throw NotFoundFile();
        }        
    }
        
    template<class T>
    T get_value(std::string& reqsec, std::string& reqvalue) {

        this->section = reqsec;
        this->value = reqvalue;
        int strok_number = 0;
        std::string iterator = "";
        std::vector <std::string> reserv = {};

        while (iterator.find(section) != true) {
            std::getline(file, iterator); 
            ++strok_number;                
            if (file.eof() == true) throw NotFoundVariable();
            
        }           
                
            std::getline(file, iterator);                              

        while (iterator.find("Section") != true) {               
            ++strok_number;            
            std::vector <std::string> st = custSplit(iterator, ' ');
                
            if (st.empty() != true) {
                reserv.push_back(st[0]);
                if (st[0] == value) {
                    if (st.size() > 1 && st[1] == "=") {
                        if (st.size() == 3 && st[2] == "") throw NotFoundValue();
                        if (st.size() > 2 && st[2] == ";") throw NotFoundValue();                         
                        else if (st.size() > 2 && st[2] != ";") {
                            if (typeid(T) == typeid(int)) {
                                int vi = stoi(st[2]);
                                return vi;
                            }
                            else if (typeid(T) == typeid(double)) {
                                double vd = stod(st[2]);
                                return vd;
                            }
                        }
                        throw NotFoundValue();
                    }
                    else throw WrongSintaksis(strok_number);
                }
            }
            st.clear();
            std::getline(file, iterator);
        }
        if ((reserv.empty() != true)) {
            std::cout << "В указанной вами секции содержаться эти переменные:\n";
            for (auto& const elem : reserv) {
                std::cout << elem << '\n';
            }
        }
        throw NotFoundVariable();
        return 0;
    }

    template<>
    std::string get_value(std::string& reqsec, std::string& reqvalue) {

        this->section = reqsec;
        this->value = reqvalue;
        int strok_number = 0;
        std::string iterator = "";
        std::vector <std::string> reserv = {};

        while (iterator.find(section) != true) {
            std::getline(file, iterator);
            ++strok_number;
            if (file.eof() == true) throw NotFoundVariable();
        }

        std::getline(file, iterator);

        while (iterator.find("Section") != true) {
            ++strok_number;
            std::vector <std::string> st = custSplit(iterator, ' ');
                     
            if (st.empty() != true) {
                reserv.push_back(st[0]);
                if (st[0] == value) {
                    if (st.size() > 1 && st[1] == "=") {
                        if (st.size() == 3 && st[2] == "") throw NotFoundValue();
                        if (st.size() > 2 && st[2] == ";") throw NotFoundValue();
                        else if (st.size() > 2 && st[2] != ";") {                            
                                std::string vs = st[2];
                                return vs;                            
                        }
                        throw NotFoundValue();
                    }
                    else throw WrongSintaksis(strok_number);
                }
            }
            st.clear();
            std::getline(file, iterator);
        }
        if ((reserv.empty() != true)) {
            std::cout << "В указанной вами секции содержаться эти переменные:\n";
            for (auto& const elem : reserv) {
                std::cout << elem << '\n';
            }
        }
        throw NotFoundVariable();
        return 0;
    }

    template<>
    std::vector<std::string> get_value(std::string& reqsec, std::string& reqvalue) {

        this->section = reqsec;
        this->value = reqvalue;
        int strok_number = 0;
        std::string iterator = "";
        std::vector <std::string> reserv = {};

        while (iterator.find(section) != true) {
            std::getline(file, iterator);
            ++strok_number;
            if (file.eof() == true) throw NotFoundVariable();
        }

        std::getline(file, iterator);

        while (iterator.find("Section") != true) {
            ++strok_number;
            std::vector <std::string> st = custSplit(iterator, ' ');
                     
            if (st.empty() != true) {
                reserv.push_back(st[0]);
                if (st[0] == value) {
                    if (st.size() > 1 && st[1] == "=") {
                        if (st.size() == 3 && st[2] == "") throw NotFoundValue();
                        if (st.size() > 2 && st[2] == ";") throw NotFoundValue();
                        else if (st.size() > 2 && st[2][0] == '(') {
                            std::vector<std::string> vv = {};                            
                            for (int i = 2; i < st.size(); ++i) {
                                vv.push_back(st[i]);                                
                                for (int j = 0; j < st[i].size(); ++j)
                                if (st[i][j] == ')') {
                                    return vv;
                                }
                            }
                            throw WrongSintaksis(strok_number);
                        }
                        throw NotFoundValue();
                    }
                    else throw WrongSintaksis(strok_number);
                }
            }
            st.clear();
            std::getline(file, iterator);
        }
        if ((reserv.empty() != true)) {
            std::cout << "В указанной вами секции содержаться эти переменные:\n";
            for (auto& const elem : reserv) {
                std::cout << elem << '\n';
            }
        }
        throw NotFoundVariable();
        return reserv;
    }

    std::vector<std::string> custSplit(std::string str, char separator) {     // - утянутая, и немного модифицированная, с просторов интернета, функция 
        std::vector < std::string > strings;
        int startIndex = 0, endIndex = 0;
        for (int i = 0; i <= str.size(); i++) {
            if (str[i] == separator || i == str.size()) {
                endIndex = i;
                std::string temp;
                temp.append(str, startIndex, endIndex - startIndex);
                strings.push_back(temp);
                startIndex = endIndex + 1;
            }
        }
        return strings;
    }

    ~Parser_ini() {
        if (file.is_open()) {
            file.close();
        }
    }
};

int main()
{
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    std::string s1 = "Section", s2 = "", v = "";     
    int t = 0;
    std::cout << "В какой секции ini файла осуществить поиск (укажите номер): ";
    std::cin >> s2;
    std::cout << std::endl;
    std::cout << "Значение какой переменной, в этой секции, вы хотите узнать: ";
    std::cin >> v;
    std::cout << std::endl;
    while (t < 1 || t > 4) {
        std::cout << "Укажите тип переменной (указывайте верный тип): 1)Целочисленная 2)Дробная 3)Слово 4)Вектор: ";
        std::cin >> t;
    }
    std::cout << std::endl;

    std::string s = s1+s2;    

    std::string f = "Config.ini";

    try {
        Parser_ini parser(f);

        if (t == 1) {
            auto vl1 = parser.get_value<int>(s, v);
            std::cout << "Значение запрашиваемой переменной: " << vl1;
        }
        else if (t == 2) {
            auto vl2 = parser.get_value<double>(s, v);
            std::cout << "Значение запрашиваемой переменной: " << vl2;
        }
        else if (t == 3) {
            auto vl3 = parser.get_value<std::string>(s, v);
            std::cout << "Значение запрашиваемой переменной: " << vl3;
        }
        else if (t == 4) {
            auto vl4 = parser.get_value<std::vector<std::string>>(s, v);
            std::cout << "Значение запрашиваемой переменной: ";
            for (auto& const elem : vl4) {
                std::cout << elem;
            }
        }
    }
    catch (WrongSintaksis& ex) {
        std::cout << ex.what() << ex.Get_stnum() <<  std::endl;
    }
    catch (NotFoundValue& ex) {
        std::cout << ex.what() << std::endl;
    }
    catch (NotFoundVariable& ex) {
        std::cout << ex.what() << std::endl;
    }
    catch (NotFoundFile& ex) {
        std::cout << ex.what() << std::endl;
    }  
       

    return 0;
}


