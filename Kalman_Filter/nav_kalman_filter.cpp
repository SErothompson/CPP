#include <iostream>
#include <Eigen/Dense>
#include <vector>
#include <Eigen/Core>

class NavigationKalmanFilter {
private:
    Eigen::MatrixXd state_;      // [x, y, vx, vy, ax, ay]
    Eigen::MatrixXd P_;          // State covariance
    Eigen::MatrixXd F_;          // State transition
    Eigen::MatrixXd H_;          // Measurement matrix
    Eigen::MatrixXd Q_;          // Process noise
    Eigen::MatrixXd R_;          // Measurement noise
    double dt_;                  // Time step

public:
    NavigationKalmanFilter(double dt = 0.1) : dt_(dt) {
        // State: [x, y, vx, vy, ax, ay]
        const int state_dim = 6;
        const int measure_dim = 2;  // GPS provides x, y positions

        // Initialize state vector
        state_ = Eigen::MatrixXd::Zero(state_dim, 1);

        // Initialize covariance matrix
        P_ = Eigen::MatrixXd::Identity(state_dim, state_dim) * 100.0;

        // State transition matrix
        F_ = Eigen::MatrixXd::Identity(state_dim, state_dim);
        // Update position based on velocity and acceleration
        F_(0,2) = dt_;     F_(0,4) = 0.5*dt_*dt_;  // x
        F_(1,3) = dt_;     F_(1,5) = 0.5*dt_*dt_;  // y
        // Update velocity based on acceleration
        F_(2,4) = dt_;     // vx
        F_(3,5) = dt_;     // vy

        // Measurement matrix (we only measure position)
        H_ = Eigen::MatrixXd::Zero(measure_dim, state_dim);
        H_(0,0) = 1.0;  // x
        H_(1,1) = 1.0;  // y

        // Process noise
        Q_ = Eigen::MatrixXd::Identity(state_dim, state_dim);
        Q_.block<2,2>(0,0) *= 0.1;    // position noise
        Q_.block<2,2>(2,2) *= 0.2;    // velocity noise
        Q_.block<2,2>(4,4) *= 0.3;    // acceleration noise

        // Measurement noise (GPS uncertainty)
        R_ = Eigen::MatrixXd::Identity(measure_dim, measure_dim) * 5.0;
    }

    void predict() {
        state_ = F_ * state_;
        P_ = F_ * P_ * F_.transpose() + Q_;
    }

    void update(const Eigen::Vector2d& measurement) {
        Eigen::MatrixXd y = measurement - H_ * state_;
        Eigen::MatrixXd S = H_ * P_ * H_.transpose() + R_;
        Eigen::MatrixXd K = P_ * H_.transpose() * S.inverse();

        state_ = state_ + (K * y);
        P_ = (Eigen::MatrixXd::Identity(state_.rows(), state_.rows()) - K * H_) * P_;
    }

    // Getters for state information
    Eigen::Vector2d getPosition() const {
        return state_.block<2,1>(0,0);
    }

    Eigen::Vector2d getVelocity() const {
        return state_.block<2,1>(2,0);
    }

    Eigen::Vector2d getAcceleration() const {
        return state_.block<2,1>(4,0);
    }

    double getSpeed() const {
        return getVelocity().norm();
    }

    double getHeading() const {
        Eigen::Vector2d vel = getVelocity();
        return std::atan2(vel(1), vel(0));
    }
};

// Example usage
int main() {
    NavigationKalmanFilter tracker;

    // Simulate GPS measurements
    std::vector<Eigen::Vector2d> gps_measurements = {
        Eigen::Vector2d(0.0, 0.0),
        Eigen::Vector2d(1.1, 0.9),
        Eigen::Vector2d(2.3, 2.1),
        Eigen::Vector2d(3.2, 2.9)
    };

    // Process measurements
    for (const auto& measurement : gps_measurements) {
        tracker.predict();
        tracker.update(measurement);

        // Get tracking results
        Eigen::Vector2d position = tracker.getPosition();
        Eigen::Vector2d velocity = tracker.getVelocity();
        double speed = tracker.getSpeed();
        double heading = tracker.getHeading() * 180.0 / M_PI;  // Convert to degrees

        std::cout << "Position: (" << position(0) << ", " << position(1) << ")" << std::endl;
        std::cout << "Velocity: (" << velocity(0) << ", " << velocity(1) << ")" << std::endl;
        std::cout << "Speed: " << speed << " m/s" << std::endl;
        std::cout << "Heading: " << heading << " degrees" << std::endl;
        std::cout << "------------------------" << std::endl;
    }

    return 0;
}
