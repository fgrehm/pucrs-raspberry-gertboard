#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

void config_gpio();
int config_serial(char * device, unsigned int baudrate);
int button_was_pressed(int button);

int main(int argc, char** argv) {
  int click_counts[3] = {0, 0, 0};
  int i;
  int fd;

  config_gpio();

  while (1) {
    int button_pressed = -1;
    for (i = 0; i < 3; i++) {
      if (!button_was_pressed(i)) continue;
      printf("Pressed %d\n", i+1);
      click_counts[i]++;
      button_pressed = i;
    }
    if (button_pressed == -1) {
      usleep(200000);
      continue;
    }

    printf("==> Button %d pressed %d time(s)\n", button_pressed+1, click_counts[button_pressed]);

    fd = config_serial("/dev/ttyAMA0", B9600);
    if (fd < 0) {
      printf("Error configuring serial device\n");
      return -1;
    }

    unsigned char msg = (unsigned char)button_pressed;
    write(fd, &msg, 1);

    msg = (unsigned char)click_counts[button_pressed];
    write(fd, &msg, 1);

    char ret[11];
    read(fd, &ret, 11);
    printf("%s\n", ret);
    close(fd);
  }
}

// device configuration function
int config_serial(char * device, unsigned int baudrate) {
  struct termios options;
  int fd;
  fd = open(device, O_RDWR | O_NOCTTY | O_NDELAY );
  if (fd < 0) {
    /*
     * Could not open the port.
     */
    perror("config_serial: Não pode abrir a serial - ");
    return -1;
  }
  fcntl(fd, F_SETFL, 0);
  /*
   * Get the current options for the port...
   */
  tcgetattr(fd, &options);
  /* sets the terminal to something like the "raw" mode */
  cfmakeraw(&options);
  /*
   * Set the baudrate...
   */
  cfsetispeed(&options, baudrate);
  cfsetospeed(&options, baudrate);
  /*
   * Enable the receiver and set local mode...
   */
  options.c_cflag |= (CLOCAL | CREAD);
  /*
   * No parit, 1 stop bit, size 8
   */
  options.c_cflag &= ~PARENB;
  options.c_cflag &= ~CSTOPB;
  options.c_cflag &= ~CSIZE;
  options.c_cflag |= CS8;
  /*
   * Clear old settings
   */
  options.c_cflag &= ~CRTSCTS;
  options.c_iflag &= ~(IXON | IXOFF | IXANY);
  /* non-caninical mode */
  options.c_lflag &= ~ICANON;
  /*
   * Set the new options for the port...
   */
  tcsetattr(fd, TCSANOW, &options);
  /* configura a tty para escritas e leituras não bloqueantes */
  //fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
  return fd;
}

void config_gpio() {
  char buff[100];
  int i;
  FILE* p_file;
  for(i = 23; i <= 25; i++) {
    // Enable pin
    p_file = fopen("/sys/class/gpio/export", "w");
    sprintf(buff, "%d", i);
    fputs(buff, p_file);
    fclose(p_file);
    // Set pin mode to output
    sprintf(buff, "/sys/class/gpio/gpio%i/direction", i);
    p_file = fopen(buff, "w");
    strcpy(buff, "in");
    fputs(buff, p_file);
    fclose(p_file);
  }
}

int button_was_pressed(int button) {
  char buff[100];
  sprintf(buff, "/sys/class/gpio/gpio%i/value", 25 - button);
  FILE* p_file = fopen(buff , "r");
  fgets(buff, 10, p_file);
  fclose(p_file);
  return buff[0] == '0';
}
