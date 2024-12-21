#include <iostream>
#include <Eigen/Dense>

class KalmanFilter {
private:
    Eigen::MatrixXd state_;              // State estimate
    Eigen::MatrixXd P_;                  // Estimate uncertainty
    Eigen::MatrixXd F_;                  // State transition matrix
    Eigen::MatrixXd H_;                  // Measurement matrix
    Eigen::MatrixXd Q_;                  // Process uncertainty
    Eigen::MatrixXd R_;                  // Measurement uncertainty
    Eigen::MatrixXd I_;                  // Identity matrix

public:
    KalmanFilter(int state_dimension, int measurement_dimension) {
        // Initialize matrices with appropriate dimensions
        state_ = Eigen::MatrixXd::Zero(state_dimension, 1);
        P_ = Eigen::MatrixXd::Identity(state_dimension, state_dimension);
        F_ = Eigen::MatrixXd::Identity(state_dimension, state_dimension);
        H_ = Eigen::MatrixXd::Identity(measurement_dimension, state_dimension);
        Q_ = Eigen::MatrixXd::Identity(state_dimension, state_dimension);
        R_ = Eigen::MatrixXd::Identity(measurement_dimension, measurement_dimension);
        I_ = Eigen::MatrixXd::Identity(state_dimension, state_dimension);
    }

    void predict() {
        // Prediction step
        state_ = F_ * state_;
        P_ = F_ * P_ * F_.transpose() + Q_;
    }

    void update(const Eigen::MatrixXd& measurement) {
        // Update step
        Eigen::MatrixXd y = measurement - (H_ * state_);
        Eigen::MatrixXd S = H_ * P_ * H_.transpose() + R_;
        Eigen::MatrixXd K = P_ * H_.transpose() * S.inverse();

        // Update state and covariance
        state_ = state_ + (K * y);
        P_ = (I_ - (K * H_)) * P_;
    }

    // Setters for filter parameters
    void setF(const Eigen::MatrixXd& F) { F_ = F; }
    void setH(const Eigen::MatrixXd& H) { H_ = H; }
    void setQ(const Eigen::MatrixXd& Q) { Q_ = Q; }
    void setR(const Eigen::MatrixXd& R) { R_ = R; }
    void setState(const Eigen::MatrixXd& state) { state_ = state; }
    void setP(const Eigen::MatrixXd& P) { P_ = P; }

    // Getter for state estimate
    Eigen::MatrixXd getState() const { return state_; }
};

// Example usage
int main() {
    // Create a 2D Kalman filter with 1D measurements
    KalmanFilter kf(2, 1);

    // Set up system matrices
    Eigen::MatrixXd F(2, 2);
    F << 1, 1,
         0, 1;
    kf.setF(F);

    Eigen::MatrixXd H(1, 2);
    H << 1, 0;
    kf.setH(H);

    // Set initial state
    Eigen::MatrixXd initial_state(2, 1);
    initial_state << 0, 0;
    kf.setState(initial_state);

    // Process measurement
    Eigen::MatrixXd measurement(1, 1);
    measurement << 1.0;
    
    // Run filter
    kf.predict();
    kf.update(measurement);

    // Get result
    Eigen::MatrixXd state = kf.getState();
    std::cout << "Estimated state:\n" << state << std::endl;

    return 0;
}
