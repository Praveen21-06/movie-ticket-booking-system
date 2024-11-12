#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <map>
#include <utility>

using namespace std;

// Structure to represent a movie
struct Movie {
    string name;
    vector<string> showtimes;
    int availableSeats;
    double price;
    int totalTicketsSold = 0;  // Track total tickets sold
    double revenue = 0.0;       // Track revenue generated
    vector<vector<bool>> seats; // Track seat availability

    Movie(string movieName, vector<string> times, int seats, double ticketPrice) {
        name = movieName;
        showtimes = times;
        availableSeats = seats;
        price = ticketPrice;
        // Initialize seats as available (true = available, false = booked)
        this->seats.resize(5, vector<bool>(10, true)); // 5 rows, 10 columns of seats
    }
};

// Structure to represent a booking
struct Booking {
    int movieIndex;
    int showtimeIndex;
    vector<pair<int, int>> seatsBooked;
    double totalPrice;
};

class MovieTicketBookingSystem {
private:
    vector<Movie> movies;
    map<string, vector<Booking>> userBookings;

public:
    MovieTicketBookingSystem() {
        movies.push_back(Movie("Avengers: Endgame", {"10:00 AM", "01:00 PM", "04:00 PM", "07:00 PM"}, 50, 12.50));
        movies.push_back(Movie("Spider-Man: No Way Home", {"11:00 AM", "02:00 PM", "05:00 PM", "08:00 PM"}, 40, 10.00));
        movies.push_back(Movie("The Batman", {"09:00 AM", "12:00 PM", "03:00 PM", "06:00 PM"}, 30, 15.00));
    }

    void displayMovies() const {
        cout << "\n------------------------------\n";
        cout << "|      Available Movies      |\n";
        cout << "------------------------------\n";
        for (int i = 0; i < movies.size(); i++) {
            cout << i + 1 << ". " << movies[i].name 
                 << " | Price: $" << fixed << setprecision(2) << movies[i].price 
                 << " (Seats Available: " << movies[i].availableSeats << ")\n";
        }
        cout << "------------------------------\n";
    }

    void displayShowtimes(int movieIndex) const {
        cout << "\n------------------------------\n";
        cout << "  Showtimes for " << movies[movieIndex].name << "\n";
        cout << "------------------------------\n";
        for (int i = 0; i < movies[movieIndex].showtimes.size(); i++) {
            cout << i + 1 << ". " << movies[movieIndex].showtimes[i] << endl;
        }
        cout << "------------------------------\n";
    }

    void displaySeatChart(int movieIndex) {
        cout << "\n------------------------------\n";
        cout << "|         Seat Chart         |\n";
        cout << "------------------------------\n";
        for (int row = 0; row < 5; row++) {
            for (int col = 0; col < 10; col++) {
                if (movies[movieIndex].seats[row][col]) {
                    cout << "[ ]"; // Empty seat
                } else {
                    cout << "[X]"; // Booked seat
                }
            }
            cout << endl;
        }
        cout << "------------------------------\n";
    }

    void bookTicket(const string &username, int movieIndex, int showtimeIndex, vector<pair<int, int>> seatsToBook) {
        // Check if all requested seats are available
        for (const auto &seat : seatsToBook) {
            if (!movies[movieIndex].seats[seat.first][seat.second]) {
                cout << "One or more selected seats are already booked.\n";
                return;
            }
        }

        // Update seat availability
        for (const auto &seat : seatsToBook) {
            movies[movieIndex].seats[seat.first][seat.second] = false;
            movies[movieIndex].availableSeats--;
        }

        double totalPrice = seatsToBook.size() * movies[movieIndex].price;
        movies[movieIndex].totalTicketsSold += seatsToBook.size();
        movies[movieIndex].revenue += totalPrice;

        cout << "\n------------------------------\n";
        cout << "|     Booking Confirmed!     |\n";
        cout << "------------------------------\n";
        cout << "Movie: " << movies[movieIndex].name << "\n";
        cout << "Showtime: " << movies[movieIndex].showtimes[showtimeIndex] << "\n";
        cout << "Seats Booked: ";
        for (const auto &seat : seatsToBook) {
            cout << "(" << seat.first + 1 << ", " << seat.second + 1 << ") ";
        }
        cout << "\nTotal Price: $" << fixed << setprecision(2) << totalPrice << "\n";
        cout << "Remaining Seats: " << movies[movieIndex].availableSeats << "\n";
        cout << "------------------------------\n";

        Booking booking = {movieIndex, showtimeIndex, seatsToBook, totalPrice};
        userBookings[username].push_back(booking);
    }

    void displayUserBookings(const string &username) const {
        auto it = userBookings.find(username);
        if (it == userBookings.end() || it->second.empty()) {
            cout << "\n------------------------------\n";
            cout << "|     No bookings found.     |\n";
            cout << "------------------------------\n";
            return;
        }

        cout << "\n-------------------------------\n";
        cout << "|         Your Bookings       |\n";
        cout << "-------------------------------\n";
        const vector<Booking> &bookings = it->second;
        for (int i = 0; i < bookings.size(); ++i) {
            const Booking &booking = bookings[i];
            cout << i + 1 << ". Movie: " << movies[booking.movieIndex].name
                 << " | Showtime: " << movies[booking.movieIndex].showtimes[booking.showtimeIndex]
                 << " | Seats: ";
            for (const auto &seat : booking.seatsBooked) {
                cout << "(" << seat.first + 1 << ", " << seat.second + 1 << ") ";
            }
            cout << " | Total Price: $" << fixed << setprecision(2) << booking.totalPrice << "\n";
        }
        cout << "------------------------------\n";
    }

    void cancelBooking(const string &username) {
        displayUserBookings(username);
        
        auto it = userBookings.find(username);
        if (it == userBookings.end() || it->second.empty()) {
            return;
        }

        cout << "Enter the booking number to cancel: ";
        int bookingNumber;
        cin >> bookingNumber;

        if (bookingNumber <= 0 || bookingNumber > it->second.size()) {
            cout << "Invalid booking number.\n";
            return;
        }

        Booking &booking = it->second[bookingNumber - 1];
        for (const auto &seat : booking.seatsBooked) {
            movies[booking.movieIndex].seats[seat.first][seat.second] = true;
            movies[booking.movieIndex].availableSeats++;
        }
        movies[booking.movieIndex].totalTicketsSold -= booking.seatsBooked.size();
        movies[booking.movieIndex].revenue -= booking.totalPrice;

        it->second.erase(it->second.begin() + bookingNumber - 1);
        cout << "Booking cancelled successfully.\n";
    }

    void addMovie() {
        string name;
        int seats;
        double price;
        int numShowtimes;

        cout << "\n-------------------------------\n";
        cout << "|        Add a New Movie      |\n";
        cout << "-------------------------------\n";
        cout << "Enter movie name: ";
        cin.ignore();
        getline(cin, name);
        cout << "Enter number of seats: ";
        cin >> seats;
        cout << "Enter price per ticket: ";
        cin >> price;

        vector<string> showtimes;
        cout << "Enter number of showtimes: ";
        cin >> numShowtimes;
        for (int i = 0; i < numShowtimes; i++) {
            string showtime;
            cout << "Enter showtime " << i + 1 << ": ";
            cin >> showtime;
            showtimes.push_back(showtime);
        }

        movies.push_back(Movie(name, showtimes, seats, price));
        cout << "Movie added successfully.\n";
    }

    void modifyMovie(int movieIndex, int newSeats) {
        movies[movieIndex].availableSeats = newSeats;
        cout << "Movie modified. Available seats updated.\n";
    }

    void viewMovieStats() const {
        cout << "\n------------------------------\n";
        cout << "|   Movie Status (Revenue)   |\n";
        cout << "------------------------------\n";
        for (const Movie &movie : movies) {
            cout << "Movie: " << movie.name 
                 << " | Tickets Sold: " << movie.totalTicketsSold
                 << " | Total Revenue: $" << fixed << setprecision(2) << movie.revenue << "\n";
        }
        cout << "------------------------------\n";
    }
};

// Function for Admin Menu
void adminMenu(MovieTicketBookingSystem &system) {
    int choice;
    while (true) {
        cout << "\n--------------------------------\n";
        cout << "|           ADMIN MENU         |\n";
        cout << "--------------------------------\n";
        cout << "1. Add a New Movie\n";
        cout << "2. Modify Movie Details\n";
        cout << "3. Movie Status (Revenue)\n";
        cout << "4. Logout\n";
        cout << "Enter choice: ";
        cin >> choice;

        switch (choice) {
            case 1: {
                system.addMovie();
                break;
            }
            case 2: {
                int movieIndex, newSeats;
                cout << "Enter movie index to modify: ";
                cin >> movieIndex;
                cout << "Enter new number of available seats: ";
                cin >> newSeats;
                system.modifyMovie(movieIndex - 1, newSeats);
                break;
            }
            case 3: {
                system.viewMovieStats();
                break;
            }
            case 4:
                cout << "Logging out...\n";
                return;
            default:
                cout << "Invalid choice, try again.\n";
        }
    }
}

// Function for User Menu
void userMenu(MovieTicketBookingSystem &system, const string &username) {
    int choice;
    while (true) {
        cout << "\n------------------------------\n";
        cout << "|          USER MENU         |\n";
        cout << "------------------------------\n";
        cout << "1. View Available Movies\n";
        cout << "2. Book Tickets\n";
        cout << "3. View Bookings\n";
        cout << "4. Cancel Booking\n";
        cout << "5. Logout\n";
        cout << "Enter choice: ";
        cin >> choice;

        switch (choice) {
            case 1: {
                system.displayMovies();
                break;
            }
            case 2: {
                int movieIndex, showtimeIndex;
                vector<pair<int, int>> seatsToBook;
                cout << "Enter movie index to book: ";
                cin >> movieIndex;
                movieIndex--;  // Adjust for zero-based indexing
                system.displayShowtimes(movieIndex);
                cout << "Enter showtime index: ";
                cin >> showtimeIndex;
                showtimeIndex--;  // Adjust for zero-based indexing
                system.displaySeatChart(movieIndex);
                cout << "Enter seats to book (row, column) separated by space (-1 to stop):\n";
                while (true) {
                    int row, col;
                    cin >> row >> col;
                    if (row == -1) break;
                    seatsToBook.push_back({row - 1, col - 1});  // Adjust for zero-based indexing
                }
                system.bookTicket(username, movieIndex, showtimeIndex, seatsToBook);
                break;
            }
            case 3: {
                system.displayUserBookings(username);
                break;
            }
            case 4: {
                system.cancelBooking(username);
                break;
            }
            case 5: {
                cout << "Logging out...\n";
                return;
            }
            default:
                cout << "Invalid choice, try again.\n";
        }
    }
}

// Function for Login Menu
void loginMenu(MovieTicketBookingSystem &system) {
    string username, password;
    int choice;
    while (true) {
        cout << "\n-------------------------------\n";
        cout << "| MOVIE TICKET BOOKING SYSTEM |\n";
        cout << "-------------------------------\n";
        cout << "1. Login as Admin\n";
        cout << "2. Login as User\n";
        cout << "3. Exit\n";
        cout << "Enter choice: ";
        cin >> choice;

        switch (choice) {
            case 1: {
                cout << "Enter Admin Username: ";
                cin >> username;
                cout << "Enter Admin Password: ";
                cin >> password;
                if (username == "admin" && password == "admin") {
                    adminMenu(system);
                } else {
                    cout << "Invalid credentials.\n";
                }
                break;
            }
            case 2: {
                cout << "Enter User Username: ";
                cin >> username;
                cout << "Enter User Password: ";
                cin >> password;
                userMenu(system, username);
                break;
            }
            case 3:
                cout << "Exiting...\n";
                return;
            default:
                cout << "Invalid choice, try again.\n";
        }
    }
}

int main() {
    MovieTicketBookingSystem system;
    loginMenu(system);
    return 0;
}
