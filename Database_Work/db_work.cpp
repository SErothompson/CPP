#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <iostream>
#include <memory>

// Database configuration class
class DBConfig {
private:
    const char* host;
    const char* user;
    const char* password;
    const char* database;

public:
    DBConfig() {
        // Best practice: Get these from environment variables
        host = std::getenv("DB_HOST");
        user = std::getenv("DB_USER");
        password = std::getenv("DB_PASSWORD");
        database = std::getenv("DB_NAME");
        
        if (!host) host = "localhost";
        if (!user) user = "root";
        if (!password) password = "zAQ!08091983";
        if (!database) database = "test";
    }

    std::string getConnectionString() const {
        return "tcp://" + std::string(host) + ":3306";
    }

    const char* getUser() const { return user; }
    const char* getPassword() const { return password; }
    const char* getDatabase() const { return database; }
};

// Database connection manager
class DBConnection {
private:
    sql::mysql::MySQL_Driver* driver;
    std::unique_ptr<sql::Connection> conn;
    DBConfig config;

public:
    DBConnection() {
        try {
            driver = sql::mysql::get_mysql_driver_instance();
            conn.reset(driver->connect(config.getConnectionString(), 
                                     config.getUser(), 
                                     config.getPassword()));

            // Create the database
            std::unique_ptr<sql::Statement> stmt(conn->createStatement());
            stmt->execute("CREATE DATABASE IF NOT EXISTS test");
            
            // Now select the database
            conn->setSchema(config.getDatabase());
        }
        catch (sql::SQLException& e) {
            std::cerr << "SQL Error: " << e.what() << std::endl;
            throw;
        }
    }

    sql::Connection* getConnection() {
        return conn.get();
    }
};

// Example model class for a User
class User {
private:
    int id;
    std::string name;
    std::string email;

public:
    User(const std::string& name, const std::string& email) 
        : name(name), email(email) {}

    // Getters
    int getId() const { return id; }
    std::string getName() const { return name; }
    std::string getEmail() const { return email; }

    // Setters
    void setId(int id) { this->id = id; }
    void setName(const std::string& name) { this->name = name; }
    void setEmail(const std::string& email) { this->email = email; }
};

// User repository class for database operations
class UserRepository {
private:
    DBConnection& dbConn;

public:
    UserRepository(DBConnection& conn) : dbConn(conn) {
        createTable();
    }

    void createTable() {
        try {
            std::unique_ptr<sql::Statement> stmt(dbConn.getConnection()->createStatement());
            stmt->execute(
                "CREATE TABLE IF NOT EXISTS users ("
                "id INT PRIMARY KEY AUTO_INCREMENT, "
                "name VARCHAR(100) NOT NULL, "
                "email VARCHAR(100) NOT NULL UNIQUE)"
            );
        }
        catch (sql::SQLException& e) {
            std::cerr << "Create table error: " << e.what() << std::endl;
            throw;
        }
    }

    void insert(const User& user) {
        try {
            std::unique_ptr<sql::PreparedStatement> pstmt(
                dbConn.getConnection()->prepareStatement(
                    "INSERT INTO users (name, email) VALUES (?, ?)"
                )
            );
            pstmt->setString(1, user.getName());
            pstmt->setString(2, user.getEmail());
            pstmt->execute();
        }
        catch (sql::SQLException& e) {
            std::cerr << "Insert error: " << e.what() << std::endl;
            throw;
        }
    }

    User findByEmail(const std::string& email) {
        try {
            std::unique_ptr<sql::PreparedStatement> pstmt(
                dbConn.getConnection()->prepareStatement(
                    "SELECT * FROM users WHERE email = ?"
                )
            );
            pstmt->setString(1, email);
            std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());

            if (res->next()) {
                User user(res->getString("name"), res->getString("email"));
                user.setId(res->getInt("id"));
                return user;
            }
            throw std::runtime_error("User not found");
        }
        catch (sql::SQLException& e) {
            std::cerr << "Query error: " << e.what() << std::endl;
            throw;
        }
    }
};

// Example usage
int main() {
    try {
        DBConnection dbConn;
        UserRepository userRepo(dbConn);

        // Create a new user
        User newUser("John Doe", "john@example.com");
        userRepo.insert(newUser);

        // Find user by email
        User foundUser = userRepo.findByEmail("john@example.com");
        std::cout << "Found user: " << foundUser.getName() << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
