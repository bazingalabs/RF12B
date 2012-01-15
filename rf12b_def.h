#define TX 1
#define RX 0
#define CS_PIN 10
#define NIRQ_PIN 2
#define nFFS_PIN 3
#define STATE_LENGTH 1
#define READ_DATA 2


// Ack protocol part
#define MAX_RETRY 3			// Maximum number of retries before failure
#define ACK_TIMEOUT 100		// Time before ack times out