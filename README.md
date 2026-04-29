# Automated Traffic Management System

This repository contains the Week 12 project for the LZPMPC005L Programming Clinic at Lancaster University Leipzig. The project aims to digitize the operations of a city's traffic department by implementing a system for vehicle registration and number plate recognition.

## Group I Members
* **Kaif** (Group Leader)
* **Alibay**

## Project Overview
This system is designed to allow vehicles to be registered with specific details such as the number plate, the owner's name, and the vehicle type. Additionally, it integrates number plate recognition to monitor vehicles that are entering and exiting major city junctions. 

### Key Features
* Registering a new vehicle in the system.
* Recognizing and logging number plates at city junctions.
* RESTful API backend returning JSON data.

## Architecture
The software utilizes a multi-tier architecture to separate the client interface from the backend services:
* **Client:** A web browser (e.g., Firefox, Google Chrome, Safari) or an API tester like Postman.
* **Gateway Server:** A Django server that acts as the sole gateway for the client to consume the backend API.
* **Backend Server:** A C++ Crow server that replaces the previous Django backend to handle core API Rest functions.
* **Database Server:** A PostgreSQL database, which will be manually populated to make vehicle number plates available to the program. 

## Project Documentation
Before programming, the software modeling
