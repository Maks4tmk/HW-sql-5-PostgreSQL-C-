#include <iostream>
#include <pqxx/pqxx>
#include <Windows.h>
#include <string>
#pragma execution_character_set("utf-8")


class DataBase {
private:

public:
    DataBase(std::string conn_string = "host=127.0.0.1 port=5432 dbname=DataBase user=test password=test") : conn{ conn_string } {
        std::cout << "База даных подключена!" << std::endl;
        creating_table();
    }


    void creating_table() {
        try {
            w.exec("CREATE TABLE IF NOT EXISTS Clients ("
                "id SERIAL PRIMARY KEY, "
                "first_name TEXT NOT NULL, "
                "last_name TEXT NOT NULL, "
                "email TEXT UNIQUE NOT NULL); "

                "CREATE TABLE IF NOT EXISTS Phone ("
                "phone_number TEXT UNIQUE NOT NULL,"
                "client_id INT NOT NULL REFERENCES Clients(id));");

            w.commit();
            std::cout << "Таблица создана!" << std::endl;
        }
        catch (const std::exception& e) {
            std::cout << e.what() << std::endl;
        }
    }

    void client_list() {
        try {
            pqxx::result c = w.exec_params("SELECT DISTINCT * "
                "FROM Clients "
                "ORDER BY Clients.id");
            w.commit();

            for (auto row : c) {
                c_id = row["id"].as<int>();
                first_name = row["first_name"].as<std::string>();
                last_name = row["last_name"].as<std::string>();
                email = row["email"].as<std::string>();
                std::cout << "\n" << c_id << ": " << first_name << " " << last_name << ", " << email << std::endl;

                pqxx::result p = w.exec_params("SELECT DISTINCT phone_number "
                    "FROM Phone "
                    "WHERE client_id = $1", c_id);
                w.commit();

                for (auto row1 : p) {
                    phone = row1["phone_number"].as<std::string>();
                    std::cout << "(" << phone << "), " ;
                }
                std::cout << std::endl;

            }
        }
        catch (const std::exception& e) {
            std::cout << e.what() << std::endl;
        }
    }

    void add_new_client(std::string first_name, std::string last_name, std::string email, std::string phone) {
        try {
            w.exec_params("INSERT INTO Clients (first_name, last_name, email) "
                "VALUES ($1, $2, $3) RETURNING id", first_name, last_name, email);
            w.commit();
            std::cout << "Клиент " << first_name << " " << last_name << " (" << email << ") " << " был добавлен" << std::endl;
            add_phone(email, phone);
        }
        catch (const std::exception& e) {
            std::cout << e.what() << std::endl;
        }
    }

    void add_phone(std::string email, std::string phone_num) {
        try {
            w.exec_params("INSERT INTO Phone (phone_number, client_id) "
                "VALUES ($1, (SELECT id FROM Clients WHERE email = $2))", phone_num, email);
            w.commit();
            std::cout << "Клиенту с э.почтой " << email << " был добавлен телефон: " << phone_num << std::endl;
        }
        catch (const std::exception& e) {
            std::cout << e.what() << std::endl;
        }
    }

    void updata_client(std::string email, std::string first_name, std::string last_name, std::string new_email) {
        try {
            w.exec_params("UPDATE Clients SET first_name = $1, last_name = $2, email = $3 WHERE email = $4", first_name, last_name, new_email, email);
            w.commit();
            std::cout << "Даные клиента с э.почтой " << email << " были изменены на:"<<std::endl;
            std::cout << first_name << last_name << email << std::endl;
        }
        catch (const std::exception& e) {
            std::cout << e.what() << std::endl;
        }
    }

    void clear_phone_list(std::string email) {
        try {
            w.exec_params("DELETE FROM Phone "
                "WHERE client_id IN "
                "(SELECT id FROM Clients WHERE email = $1)", email);
            w.commit();
            std::cout << "Все телефоны клиента с э.почтой " << email << " были удалены!" << std::endl;
        }
        catch (const std::exception& e) {
            std::cout << e.what() << std::endl;
        }
    }

    void delete_phone(std::string phone_num) {
        try {
            w.exec_params("DELETE FROM Phone WHERE phone_number = $1", phone_num);
            w.commit();
            std::cout << "Телефонный номер (" << phone_num << ") был удалён!" << std::endl;
        }
        catch (const std::exception& e) {
            std::cout << e.what() << std::endl;
        }
    }

    void delete_client(std::string email) {
        try {
            clear_phone_list(email);
            w.exec_params("DELETE FROM Clients WHERE email = $1", email);
            w.commit();
            std::cout << "Клиент с э.почтой " << email << " был удалён!" << std::endl;
        }
        catch (const std::exception& e) {
            std::cout << e.what() << std::endl;
        }
    }

    void search_client(std::string query) {
        try {
            pqxx::result c = w.exec_params("SELECT DISTINCT * "
                "FROM Clients "
                "LEFT JOIN  public.Phone ON public.Clients.id = public.Phone.client_id "
                "WHERE Clients.first_name = $1 OR "
                "Clients.last_name = $1 OR "
                "Clients.email = $1 OR "
                "Phone.phone_number = $1;", query);
            w.commit();
            for (auto row : c) {
                c_id = row["id"].as<int>();
                first_name = row["first_name"].as<std::string>();
                last_name = row["last_name"].as<std::string>();
                email = row["email"].as<std::string>();
                std::cout << "Результаты по запросу '" << query << "'" << "\n";
                std::cout << c_id << ": " << first_name << " " << last_name << ", " << email << std::endl;

                pqxx::result p = w.exec_params("SELECT DISTINCT phone_number "
                    "FROM Phone "
                    "WHERE client_id = $1", c_id);
                w.commit();

                for (auto row1 : p) {
                    phone = row1["phone_number"].as<std::string>();
                    std::cout << "(" << phone << "), ";
                }
                std::cout << std::endl;
            }
        }
        catch (const std::exception& e) {
            std::cout << e.what() << std::endl;
        }
    }

private:
    std::string first_name;
    std::string last_name;
    std::string email;
    std::string phone;
    int c_id;

    pqxx::connection conn;
    
    pqxx::work w{ conn };
    
};


int main (){
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);


    DataBase a;
    a.add_new_client("Miki", "Mouse", "mik.mouse@email.com", "89988072323");
    a.add_new_client("Mini", "Mouse", "min.mouse@email.com", "89988072331");
    a.add_new_client("Donald", "Duck", "Duck.Duck@email.com", "89988072341");

    a.add_phone("mik.mouse@email.com", "89988072324");
    a.add_phone("min.mouse@email.com", "89988072335");
    a.add_phone("mik.mouse@email.com", "89988072326");

    a.client_list();

    a.delete_phone("89988072326");
    a.clear_phone_list("min.mouse@email.com");

    a.delete_client("Duck.Duck@email.com");

    a.client_list();

    a.add_new_client("Donald", "Duck", "Duck.Duck@email.com", "89988072341");

    a.client_list();
    a.updata_client("Duck.Duck@email.com", "Gufi", "Gufich", "gufi@email.com");

    a.client_list();

    a.search_client("Mini");
    a.search_client("Mouse""Mini");
    a.search_client("min.mouse@email.com");
    a.search_client("89988072323");
}
