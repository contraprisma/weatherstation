#include <pgmspace.h>

#define SECRET
// Make sure this is unique for each device you want to connect to and link to the certificate and private key.
#define THINGNAME "..."

// Your own Wifi access point and password.
const char WIFI_SSID[] = "...";
const char WIFI_PASSWORD[] = "...";

// The AWS IoT endpoint. Ensure this is https.
const char AWS_IOT_ENDPOINT[] = "...";

// Amazon Root CA 1
static const char AWS_CERT_CA[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
...
-----END CERTIFICATE-----
)EOF";

// Device Certificate
static const char AWS_CERT_CRT[] PROGMEM = R"KEY(
-----BEGIN CERTIFICATE-----
...
-----END CERTIFICATE-----
)KEY";

// Device Private Key
static const char AWS_CERT_PRIVATE[] PROGMEM = R"KEY(
-----BEGIN RSA PRIVATE KEY-----
...
-----END RSA PRIVATE KEY-----
)KEY";