#define IP_SIZE     16
#define MAX_PATH_SIZE 4096
#define MAX_NAME_SIZE   1024
#define MAX_ENTRIES     1024
#define str     char*
#define str_arr char**
#define PARAMS(x) &x, sizeof(x), 0
#define SPARAM(x) x, strlen(x), 0
#define NM_CLIENT_PORT  5050 
#define NM_SERVER_PORT  6060 
#define MAX_INPUT_SIZE 5000 
#define MAX_WORDS   512
#define CACHE_SIZE 5

#define CRT     (1 << 1)
#define DLT     (1 << 2)
#define CPY     (1 << 3)
#define MOV     (1 << 4)
#define WRT     (1 << 5)
#define RED     (1 << 6)
#define INF     (1 << 7)