#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <net/if.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <signal.h>
#include <pthread.h>

#define CAN_INTERFACE "vcan0"
#define MSG_LENGTH 8
#define SERVICE_1 0x41
#define SERVICE_2 0x42
#define SERVICE_3 0x43
#define SERVICE_7 0x47
#define SLEEP_TIME 1
#define TESTING 1

int socket_id = 0;
int mil_status;
struct send_struct {
    int* data;
    int socket_id;
    int length;
};

int send_obd_message(int s, int data[], int length);
int receive_obd_message(int s, int service);


/**
 	@brief Closes the socket before stoping the program
	@param _ unused value
*/
static void sig_handler(int _) {
  (void)_;
  printf("\nHalting program\n");
  if (close(socket_id) < 0) {
		perror("Error closing the Socket");
		exit(EXIT_FAILURE);
	}
  exit(EXIT_SUCCESS);
}


/**
 	@brief Creates the connection with the CAN network
	@return socket ID
*/
int setup_socket() {
  int s;
  struct sockaddr_can addr;
  struct ifreq ifr;

  // Create a socket
  if ((s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
    perror("Socket creation error");
    exit(1);
  }

  strcpy(ifr.ifr_name, CAN_INTERFACE);
  ioctl(s, SIOCGIFINDEX, &ifr);

  memset(&addr, 0, sizeof(addr));
  addr.can_family = AF_CAN;
  addr.can_ifindex = ifr.ifr_ifindex;
  if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    perror("Bind error");
    return EXIT_FAILURE;
  }
  return s;
}


/**
 	@brief Creates thread to send request data
	@param arg saves the values of the socket_id, length and data
	@return NULL
*/
void* send_thread(void* arg) {
  struct send_struct* params = (struct send_struct*)arg;
  int* data = params->data;
  int socket_id = params->socket_id;
  int length = params->length;

  send_obd_message(socket_id, data, length);
  return NULL;
}


/**
 	@brief Creates thread to receive CAN data
	@param arg saves the id of the service to which data will be received
	@return NULL
*/
void* receive_thread(void* arg) {
  int* service = (int*)arg;
  mil_status = receive_obd_message(socket_id, *service);
  return NULL;
}


/**
 	@brief Creates thread to receive CAN data
	@param data message information sent through CAN frames
  @param socket_id id of the socket
	@param service service to consult
	@param send_t reference of send thread
	@param receive_t reference of receive thread

*/
void execute_service(int *data, int socket_id, int service, pthread_t *send_t, pthread_t *receive_t) {
  struct send_struct params = {data, socket_id, MSG_LENGTH};
  if (pthread_create(receive_t, NULL, receive_thread, &service) != 0) {
    perror("Failed to create receive thread");
    exit(EXIT_FAILURE);
  }

  if (pthread_create(send_t, NULL, send_thread, (void*)&params) != 0) {
    perror("Failed to create send thread");
    exit(EXIT_FAILURE);
  }
  
  pthread_join(*send_t, NULL);
  pthread_join(*receive_t, NULL);
}

// [#bytes, mode, PID, A, B, C, D]
/**
 	@brief Entry point of the code as well as top level logic
*/
int main() {
  socket_id = setup_socket();
  signal(SIGINT, sig_handler);
  pthread_t send_t, receive_t;
  if (TESTING){
    printf("System running on Testing mode\n");
    receive_obd_message(socket_id, SERVICE_3);
  } else {
    while(1) {
      int data_s1[7] = {0x02, 0x01, 0x01, 0x55, 0x55, 0x55, 0x55};
      execute_service(data_s1, socket_id, SERVICE_1, &send_t, &receive_t);
      if(mil_status == 2) {
        int data_s2[7] = {0x05, 0x02, 0x01, 0x00, 0x02, 0x00, 0x55};
        execute_service(data_s2, socket_id, SERVICE_2, &send_t, &receive_t);

        int data_s3[7] = {0x01, 0x03, 0x55, 0x55, 0x55, 0x55, 0x55};
        execute_service(data_s3, socket_id, SERVICE_3, &send_t, &receive_t);
      } else if (mil_status == 0) {
        int data_s7[7] = {0x01, 0x07, 0x55, 0x55, 0x55, 0x55, 0x55};
        execute_service(data_s7, socket_id, SERVICE_7, &send_t, &receive_t);
      }
      sleep(SLEEP_TIME);
    }
  }  
  if (close(socket_id) < 0) {
		perror("Error closing the Socket");
		exit(EXIT_FAILURE);
	}
  return EXIT_SUCCESS;
}
