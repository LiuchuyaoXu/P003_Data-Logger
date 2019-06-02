#include <iostream>

#include <fcntl.h>
#include <unistd.h>
#include <termios.h>

#include "sliding_dft.hpp"

using namespace std;

int open_serial_port()
{
    // Open serial port using C++ standard library.
    // string serial_port {"/dev/ttyACM0"};
    // fstream fs {serial_port, ios_base::in};
    // if (!fs.is_open()) cout << "Failed, could not open " << serial_port << "." << endl;

    // Open serial port.
    // For serial port options, read Advanced Programming in the UNIX Environment Chapter 3 File I/O.
    char serial_port[] {"/dev/ttyACM2"};
    int serial_port_options {O_RDWR | O_NOCTTY};
    int fd {open(serial_port, serial_port_options)};
    if (fd == -1) {cout << "Failed, could not open " << serial_port << "." << endl;}

    // Set serial port attributes.
    // For serial port attributes, read Advanced Programming in the UNIX Environment Chapter 18 Terminal I/O.
    termios serial_port_attributes {};
    tcgetattr(fd, &serial_port_attributes);
    serial_port_attributes.c_cflag = B9600 | CRTSCTS | CS8 | CLOCAL | CREAD;
    serial_port_attributes.c_iflag = IGNPAR | ICRNL;
    serial_port_attributes.c_oflag = 0;
    serial_port_attributes.c_lflag = ICANON;
    int result {tcsetattr(fd, TCSANOW, &serial_port_attributes)};
    if (result != 0) {cout << "Failed, could not set " << serial_port << " attributes." << endl;}

    return fd;
}

void close_serial_port(int fd)
{
    close(fd);
}

int read_serial_port(int fd)
{
    // Read from the serial port and fill the buffer.
    char buff[5] {};
    int bytes_read = read(fd, buff, 5);
    if (bytes_read == -1) {cout << "Failed, could not read from the Arduino." << endl;}

    // Convert the readings into integer.
    int result {};
    if (bytes_read == 5) {
        result = (buff[0] - '0') * 1000 + (buff[1] - '0') * 100 + (buff[2] - '0') * 10 + (buff[3] - '0');
    }
    else if (bytes_read == 4) {
        result = (buff[0] - '0') * 100 + (buff[1] - '0') * 10 + (buff[2] - '0');
    }
    else if (bytes_read == 3) {
        result = (buff[0] - '0') * 10 + (buff[1] - '0');
    }
    else if (bytes_read == 2) {
        result = (buff[0] - '0');
    }
    else {
        result = -1;
    }
    return result;
}

int main(int argc, char** argv)
{
    // Open the Arduino serial port.
    auto fd = open_serial_port();

    // Discard invalid readings from Arduino.
    while (read_serial_port(fd) < 0) {
        cout << "Discarded, invalid reading from Arduino." << endl;
    }

    int reading {};
    static SlidingDFT<double, 512> dft;
    complex<double> DC_bin;
    while (1) {
        reading = read_serial_port(fd);
        dft.update(double(reading));
        DC_bin = dft.dft[250];
        cout << abs(DC_bin) << endl;
    }

    // Close the Arduino serial port.
    close_serial_port(fd);
    return 0;
}
