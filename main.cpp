#include "crow.h"
#include <mysql.h>
#include <iostream>
#include <string>

// Helper to open a connection using the standard C API
MYSQL* get_db_connection() {
    MYSQL* conn = mysql_init(NULL);
    if (conn == nullptr) {
        std::cerr << "MySQL Initialization failed" << std::endl;
        return nullptr;
    }
    // Connect via TCP to local DB (Uses your root credentials, traffic_db)
    if (mysql_real_connect(conn, "127.0.0.1", "root", "", "traffic_db", 3306, NULL, 0) == NULL) {
        std::cerr << "Database Connection Error: " << mysql_error(conn) << std::endl;
        mysql_close(conn);
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
                            + plate + "', '" + owner + "', '" + type + "')";
        
        if (mysql_query(conn, query.c_str())) {
            std::cerr << "Registration Error: " << mysql_error(conn) << std::endl;
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
        std::string select_query = "SELECT owner_name, vehicle_type FROM vehicles WHERE number_plate = '" + plate + "'";
        if (mysql_query(conn, select_query.c_str())) {
            mysql_close(conn);
            return crow::response(500, "Query Error");
        }

        MYSQL_RES* result = mysql_store_result(conn);
        if (result == nullptr || mysql_num_rows(result) == 0) {
            if (result) mysql_free_result(result);
            mysql_close(conn);
            return crow::response(404, "Plate not found in database");
        }

        MYSQL_ROW row = mysql_fetch_row(result);
        std::string owner_name = row[0] ? row[0] : "Unknown";
        std::string vehicle_type = row[1] ? row[1] : "Unknown";
        mysql_free_result(result);

        // 2. Log the detection
        std::string log_query = "INSERT INTO junction_logs (number_plate, junction_location) VALUES ('" + plate + "', '" + junction + "')";
        mysql_query(conn, log_query.c_str());

        mysql_close(conn);

        // 3. Build success response
        crow::json::wvalue res;
        res["status"] = "Success";
        res["recognized"] = true;
        res["number_plate"] = plate;
        res["owner_name"] = owner_name;
        res["vehicle_type"] = vehicle_type;
        res["junction"] = junction;
        return crow::response(201, res);
    });

    // -----------------------------
    // POST /violation
    // -----------------------------
    CROW_ROUTE(app, "/violation").methods("POST"_method)
    ([](const crow::request& req){
        auto data = crow::json::load(req.body);
        if(!data) return crow::response(400, "Invalid JSON");

        std::string plate = data["number_plate"].s();
        std::string v_type = data["violation_type"].s();
        int severity = data.has("severity") ? data["severity"].i() : 1;
    std::string location = "Unknown";
if (data.has("location")) {
    location = data["location"].s();
}
        double fine_amount = 0.0;
        if (v_type == "SPEEDING") fine_amount = 50.0 * severity;
        else if (v_type == "RED_LIGHT") fine_amount = 200.0;
        else if (v_type == "ILLEGAL_PARKING") fine_amount = 30.0 * severity;
        else return crow::response(400, "Unknown violation type");

        MYSQL* conn = get_db_connection();
        if (!conn) return crow::response(500, "Database Error");

        // 1. Insert violation
        std::string v_query = "INSERT INTO violations (number_plate, violation_type, severity, location) VALUES ('" 
                              + plate + "', '" + v_type + "', " + std::to_string(severity) + ", '" + location + "')";
        if (mysql_query(conn, v_query.c_str())) {
            mysql_close(conn);
            return crow::response(500, "Database Error saving violation");
        }

        // 2. Get the auto-incremented ID
        int violation_id = mysql_insert_id(conn);

        // 3. Issue the Fine
        std::string f_query = "INSERT INTO fines (violation_id, amount, status) VALUES (" 
                              + std::to_string(violation_id) + ", " + std::to_string(fine_amount) + ", 'UNPAID')";
        mysql_query(conn, f_query.c_str());

        // 4. Handle Email (Fetch owner name)
        std::string o_query = "SELECT owner_name FROM vehicles WHERE number_plate = '" + plate + "'";
        std::string owner_name = "Unknown";
        std::string simulated_email = "owner@example.com";

        if (!mysql_query(conn, o_query.c_str())) {
            MYSQL_RES* o_res = mysql_store_result(conn);
            if (o_res && mysql_num_rows(o_res) > 0) {
                MYSQL_ROW o_row = mysql_fetch_row(o_res);
                if (o_row[0]) owner_name = o_row[0];
                
                // Replace spaces with dots for the email
                std::string email_name = owner_name;
                for (char& c : email_name) if (c == ' ') c = '.';
                simulated_email = email_name + "@traffic-system.local";
            }
            if (o_res) mysql_free_result(o_res);
        }

        mysql_close(conn);

        crow::json::wvalue response;
        response["status"] = "Violation Processed successfully";
        response["vehicle"] = plate;
        response["violation_type"] = v_type;
        response["severity_level"] = severity;
        response["fine_issued_eur"] = fine_amount;
        response["notification_status"] = "Email notification triggered to: " + simulated_email;

        return crow::response(201, response);
    });

    app.port(8080).multithreaded().run();
}