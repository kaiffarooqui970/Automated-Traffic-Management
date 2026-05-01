-- schema.sql

-- 1. Owners Table
CREATE TABLE IF NOT EXISTS owners (
    owner_id INT AUTO_INCREMENT PRIMARY KEY,
    first_name VARCHAR(100) NOT NULL,
    last_name VARCHAR(100) NOT NULL,
    email VARCHAR(150) UNIQUE NOT NULL,
    phone_number VARCHAR(20)
);

-- 2. Vehicles Table (Merged)
CREATE TABLE IF NOT EXISTS vehicles (
    number_plate VARCHAR(20) PRIMARY KEY, -- Using number_plate as the unique identifier
    owner_id INT NOT NULL,
    vehicle_type VARCHAR(50),             -- Brought over from your original table
    make VARCHAR(50),
    model VARCHAR(50),
    color VARCHAR(30),
    FOREIGN KEY (owner_id) REFERENCES owners(owner_id) ON DELETE CASCADE
);

-- 3. Junction Logs Table (Updated for MySQL)
CREATE TABLE IF NOT EXISTS junction_logs (
    log_id INT AUTO_INCREMENT PRIMARY KEY,
    number_plate VARCHAR(20) NOT NULL,
    junction_location VARCHAR(100) NOT NULL,
    log_timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (number_plate) REFERENCES vehicles(number_plate) ON DELETE CASCADE
);

-- 4. Violations Table
CREATE TABLE IF NOT EXISTS violations (
    violation_id INT AUTO_INCREMENT PRIMARY KEY,
    number_plate VARCHAR(20) NOT NULL,
    violation_type ENUM('SPEEDING', 'RED_LIGHT', 'ILLEGAL_PARKING') NOT NULL,
    severity INT DEFAULT 1, 
    location VARCHAR(255) NOT NULL,
    timestamp DATETIME DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (number_plate) REFERENCES vehicles(number_plate) ON DELETE CASCADE
);

-- 5. Fines Table
CREATE TABLE IF NOT EXISTS fines (
    fine_id INT AUTO_INCREMENT PRIMARY KEY,
    violation_id INT NOT NULL,
    amount DECIMAL(10, 2) NOT NULL,
    status ENUM('UNPAID', 'PAID', 'DISMISSED') DEFAULT 'UNPAID',
    issued_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (violation_id) REFERENCES violations(violation_id) ON DELETE CASCADE
);

-- Insert test owners
INSERT INTO owners (first_name, last_name, email) 
VALUES 
    ('John', 'Doe', 'john.doe@example.com'),
    ('Jane', 'Smith', 'jane.smith@example.com');

-- Insert test vehicles (Assuming John is owner_id 1, and Jane is owner_id 2)
INSERT INTO vehicles (number_plate, owner_id, vehicle_type) 
VALUES 
    ('LEI-AB 123', 1, 'Sedan'),
    ('B-XY 987', 2, 'SUV');