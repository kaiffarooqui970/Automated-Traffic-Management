#include "crow.h"
#include <unordered_map>
#include <string>

struct Vehicle {
    std::string number_plate;
    std::string owner;
    std::string type;
};

// In-memory database for testing (Replace with PostgreSQL logic later)
std::unordered_map<std::string, Vehicle> database;

int main() {
    crow::SimpleApp app;

    // -----------------------------
    // POST /register
    // -----------------------------
    CROW_ROUTE(app, "/register").methods("POST"_method)
    ([](const crow::request& req){
        auto data = crow::json::load(req.body);

        if(!data)
            return crow::response(400, "Invalid JSON");

        Vehicle v{
            data["number_plate"].s(),
            data["owner_name"].s(),
            data["vehicle_type"].s()
        };

        database[v.number_plate] = v;

        crow::json::wvalue res;
        res["status"] = "registered";
        res["plate"] = v.number_plate;

        // Explicitly return 201 Created to pass the Postman test
        return crow::response(201, res);
    });

    // -----------------------------
    // POST /recognize
    // -----------------------------
    CROW_ROUTE(app, "/recognize").methods("POST"_method)
    ([](const crow::request& req){
        auto data = crow::json::load(req.body);

        if(!data)
            return crow::response(400, "Invalid JSON");

        std::string p = data["number_plate"].s();

        if(database.count(p) == 0)
            return crow::response(404, "Plate not found in database");

        const Vehicle &v = database[p];

        crow::json::wvalue res;
        res["status"] = "Success";
        res["recognized"] = true;
        res["number_plate"] = v.number_plate;
        res["owner_name"] = v.owner;
        res["vehicle_type"] = v.type;
        
        // If the junction location was sent, log it in the response
        if (data.has("junction_location")) {
            res["junction"] = data["junction_location"].s();
        }

        // Explicitly return 201 Created to pass the Postman test
        return crow::response(201, res);
    });

    // Corrected port to 8080 to match Django's target
    app.port(8080).multithreaded().run();
}