#include "crow.h"
#include <mysql.h>
#include <iostream>
#include <string>

// Helper function to open a connection to your Mac's MySQL database
MYSQL* get_db_connection() {
    MYSQL *conn = mysql_init(NULL);
    // Uses your exact working credentials: root, no password, traffic_db
    if (!mysql_real_connect(conn, "localhost", "root", "", "traffic_db", 0, NULL, 0)) {
        std::cerr << "Database Connection Error: " << mysql_error(conn) << std::endl;
        return nullptr;
    }
    return conn;
}

int main() {
    crow::SimpleApp app;

    // -----------------------------
    // POST /register
    // -----------------------------
    CROW_ROUTE(app, "/register").methods("POST"_method)
    ([](const crow::request& req){
        auto data = crow::json::load(req.body);

        if(!data) return crow::response(400, "Invalid JSON");

        std::string plate = data["number_plate"].s();
        std::string owner = data["owner_name"].s();
        std::string type = data["vehicle_type"].s();

        MYSQL* conn = get_db_connection();
        if (!conn) return crow::response(500, "Database Error");
    
        std::string query = "INSERT INTO vehicles (number_plate, owner_name, vehicle_type) VALUES ('" 
                        + plate + "', '" + owner + "', '" + type + "');";
        if (mysql_query(conn, query.c_str())) {
            std::cerr << "Registration SQL Error: " << mysql_error(conn) << std::endl;
            mysql_close(conn);
            return crow::response(400, "Failed to register vehicle. Plate might already exist.");
        }

        mysql_close(conn);

        crow::json::wvalue res;
        res["status"] = "registered";
        res["plate"] = plate;

        return crow::response(201, res);
    });

    // -----------------------------
    // POST /recognize
    // -----------------------------
    CROW_ROUTE(app, "/recognize").methods("POST"_method)
    ([](const crow::request& req){
        auto data = crow::json::load(req.body);

        if(!data) return crow::response(400, "Invalid JSON");

        std::string plate = data["number_plate"].s();
        std::string junction = "Unknown";
        if (data.has("junction_location")) {
            junction = data["junction_location"].s();
        }
        MYSQL* conn = get_db_connection();
        if (!conn) return crow::response(500, "Database Error");

        // 1. Check if the vehicle exists
        std::string select_query = "SELECT owner_name, vehicle_type FROM vehicles WHERE number_plate = '" + plate + "';";
        
        if (mysql_query(conn, select_query.c_str())) {
            std::cerr << "Recognize Select Error: " << mysql_error(conn) << std::endl;
            mysql_close(conn);
            return crow::response(500, "Query Error");
        }

        MYSQL_RES* result = mysql_store_result(conn);
        if (result == nullptr || mysql_num_rows(result) == 0) {
            if (result) mysql_free_result(result);
            mysql_close(conn);
            return crow::response(404, "Plate not found in database");
        }

        // Fetch the data
        MYSQL_ROW row = mysql_fetch_row(result);
        std::string owner_name = row[0] ? row[0] : "Unknown";
        std::string vehicle_type = row[1] ? row[1] : "Unknown";
        mysql_free_result(result);

        // 2. Log the detection
        std::string insert_log_query = "INSERT INTO junction_logs (number_plate, junction_location) VALUES ('" 
                                       + plate + "', '" + junction + "');";
        
        if (mysql_query(conn, insert_log_query.c_str())) {
            std::cerr << "Logging SQL Error: " << mysql_error(conn) << std::endl;
        }

        mysql_close(conn);

        // 3. Build the success response
        crow::json::wvalue res;
        res["status"] = "Success";
        res["recognized"] = true;
        res["number_plate"] = plate;
        res["owner_name"] = owner_name;
        res["vehicle_type"] = vehicle_type;
        res["junction"] = junction;

        return crow::response(201, res);
    });

    app.port(8080).multithreaded().run();
}