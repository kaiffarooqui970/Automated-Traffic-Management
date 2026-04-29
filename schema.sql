-- Table for registered vehicles
CREATE TABLE vehicles (
    id SERIAL PRIMARY KEY,
    number_plate VARCHAR(20) UNIQUE NOT NULL,
    owner_name VARCHAR(100) NOT NULL,
    vehicle_type VARCHAR(50) NOT NULL
);

-- Table for logging recognized plates at junctions
CREATE TABLE junction_logs (
    id SERIAL PRIMARY KEY,
    number_plate VARCHAR(20) NOT NULL,
    junction_location VARCHAR(100) NOT NULL,
    log_timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (number_plate) REFERENCES vehicles(number_plate)
);

-- Manually populating some test data
INSERT INTO vehicles (number_plate, owner_name, vehicle_type) 
VALUES ('LEI-AB 123', 'John Doe', 'Sedan'),
       ('B-XY 987', 'Jane Smith', 'SUV');
