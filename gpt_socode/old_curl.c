#include <stdio.h>
#include <curl/curl.h>

// Callback function for handling received data
size_t write_callback(char* ptr, size_t size, size_t nmemb, void* userdata) {
    printf("%.*s", size * nmemb, ptr);
    fflush(stdout);
    return size * nmemb;
}

int main() {
    CURL* curl;
    CURLcode res;

    curl = curl_easy_init();
    if (curl) {
        // Set the URL to request
        curl_easy_setopt(curl, CURLOPT_URL, "https://example.com");

        // Set the callback function for received data
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);

        // Make the request
        res = curl_easy_perform(curl);

        // Check for errors
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }

        // Clean up
        curl_easy_cleanup(curl);
    }

    return 0;
}

int main2(int argc, char* argv[]) {
    CURL* curl;
    CURLcode res;
    FILE* file;
    struct curl_httppost* formpost = NULL;
    struct curl_httppost* lastptr = NULL;
    struct curl_slist* headerlist = NULL;
    static const char buf[] = "Expect:";

    // Check for filename argument
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    // Open the file to upload
    file = fopen(argv[1], "rb");
    if (!file) {
        fprintf(stderr, "Failed to open file: %s\n", argv[1]);
        return 1;
    }

    // Initialize libcurl
    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    if (curl) {
        // Set the URL to upload to
        curl_easy_setopt(curl, CURLOPT_URL, "http://example.com/upload");

        // Add the file as a form post field
        curl_formadd(&formpost, &lastptr, CURLFORM_COPYNAME, "file",
            CURLFORM_FILE, argv[1], CURLFORM_END);

        // Set the HTTP headers
        headerlist = curl_slist_append(headerlist, buf);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist);

        // Set the form data
        curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);

        // Perform the HTTP POST request
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }

        // Clean up
        curl_slist_free_all(headerlist);
        curl_easy_cleanup(curl);
        curl_formfree(formpost);
    }

    // Close the file
    fclose(file);

    return 0;
}

int main3(int argc, char* argv[]) {
    CURL* curl;
    FILE* file;
    CURLcode res;

    // Check for URL and filename arguments
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <url> <filename>\n", argv[0]);
        return 1;
    }

    // Open the file to save to
    file = fopen(argv[2], "wb");
    if (!file) {
        fprintf(stderr, "Failed to create file: %s\n", argv[2]);
        return 1;
    }

    // Initialize libcurl
    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    if (curl) {
        // Set the URL to download from
        curl_easy_setopt(curl, CURLOPT_URL, argv[1]);

        // Set the file to save to
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);

        // Perform the HTTP GET request
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }

        // Clean up
        curl_easy_cleanup(curl);
    }

    // Close the file
    fclose(file);

    return 0;
}

int main4(int argc, char* argv[]) {
    CURL* curl;
    CURLcode res;
    char* url;

    // Check for URL argument
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <url>\n", argv[0]);
        return 1;
    }

    // Get the URL from the command line arguments
    url = argv[1];

    // Initialize libcurl
    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    if (curl) {
        // Set the URL to fetch
        curl_easy_setopt(curl, CURLOPT_URL, url);

        // Follow any redirects
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

        // Set the callback function for writing the response to stdout
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, fwrite);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, stdout);

        // Perform the HTTP GET request
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }

        // Clean up
        curl_easy_cleanup(curl);
    }

    // Cleanup global libcurl state
    curl_global_cleanup();

    return 0;
}

int main5(int argc, char* argv[]) {
    CURL* curl;
    CURLcode res;
    struct curl_httppost* formpost = NULL;
    struct curl_httppost* lastptr = NULL;

    // Check for file and URL arguments
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <file> <url>\n", argv[0]);
        return 1;
    }

    // Initialize libcurl
    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    if (curl) {
        // Set the URL to upload to
        curl_easy_setopt(curl, CURLOPT_URL, argv[2]);

        // Add the file to the POST request
        curl_formadd(&formpost, &lastptr,
            CURLFORM_COPYNAME, "file",
            CURLFORM_FILE, argv[1],
            CURLFORM_END);

        // Set the POST request data
        curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);

        // Perform the HTTP POST request
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }

        // Clean up
        curl_formfree(formpost);
        curl_easy_cleanup(curl);
    }

    // Cleanup global libcurl state
    curl_global_cleanup();

    return 0;
}

int main6(int argc, char* argv[]) {
    CURL* curl;
    CURLcode res;
    char* url;

    // Check for URL argument
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <url>\n", argv[0]);
        return 1;
    }

    // Get the URL from the command line arguments
    url = argv[1];

    // Initialize libcurl
    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    if (curl) {
        // Set the URL to fetch
        curl_easy_setopt(curl, CURLOPT_URL, url);

        // Set the callback function for writing the response to stdout
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);

        // Set the timeout to 0 to allow long polling
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 0L);

        // Loop until the request is canceled
        bool canceled = false;
        while (!canceled) {
            // Perform the HTTP GET request
            res = curl_easy_perform(curl);
            if (res != CURLE_OK) {
                fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            }

            // Wait a short time before making another request
            sleep(1);
        }

        // Clean up
        curl_easy_cleanup(curl);
    }

    // Cleanup global libcurl state
    curl_global_cleanup();

    return 0;
}

int main7(int argc, char* argv[]) {
    CURL* curl;
    CURLcode res;
    char* url;
    FILE* file;
    struct stat file_info;

    // Check for file and URL arguments
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <file> <url>\n", argv[0]);
        return 1;
    }

    // Get the file and URL from the command line arguments
    file = fopen(argv[1], "rb");
    if (!file) {
        fprintf(stderr, "Could not open file: %s\n", argv[1]);
        return 1;
    }
    url = argv[2];

    // Initialize libcurl
    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    if (curl) {
        // Set the URL to upload to
        curl_easy_setopt(curl, CURLOPT_URL, url);

        // Set the PUT request method
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");

        // Set the file size for the request
        if (fstat(fileno(file), &file_info) != 0) {
            fprintf(stderr, "Could not get file size: %s\n", argv[1]);
            return 1;
        }
        curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, (curl_off_t)file_info.st_size);

        // Set the input file for the request
        curl_easy_setopt(curl, CURLOPT_READDATA, file);

        // Perform the HTTP PUT request
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }

        // Clean up
        curl_easy_cleanup(curl);
    }

    // Cleanup global libcurl state
    curl_global_cleanup();

    return 0;
}

int main8(void) {
    CURL* curl;
    CURLcode res;
    char* url = "https://example.com/api";
    char* json_data = "{\"name\": \"John Doe\", \"age\": 30}";

    // Initialize libcurl
    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    if (curl) {
        // Set the URL to POST to
        curl_easy_setopt(curl, CURLOPT_URL, url);

        // Set the POST request method
        curl_easy_setopt(curl, CURLOPT_POST, 1L);

        // Set the JSON data for the request
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_data);

        // Set the content type for the request
        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // Set the callback to receive the response body
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);

        // Set the response body data
        struct response_data response = { 0 };
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        // Perform the HTTP POST request
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }

        // Print the response body to standard output
        printf("%s\n", response.data);

        // Clean up
        curl_easy_cleanup(curl);
        free(response.data);
    }

    // Cleanup global libcurl state
    curl_global_cleanup();

    return 0;
}

int main9(void) {
    CURL* curl;
    CURLcode res;
    char* url = "https://example.com/upload";
    char* filename = "example.txt";

    // Initialize libcurl
    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    if (curl) {
        // Set the URL to PUT to
        curl_easy_setopt(curl, CURLOPT_URL, url);

        // Set the PUT request method
        curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

        // Set the file to upload
        curl_easy_setopt(curl, CURLOPT_READDATA, fopen(filename, "rb"));

        // Set the file size for the request
        long filesize;
        FILE* fp = fopen(filename, "rb");
        if (fp) {
            fseek(fp, 0L, SEEK_END);
            filesize = ftell(fp);
            fclose(fp);
            curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, filesize);
        }
        else {
            fprintf(stderr, "Failed to open file %s\n", filename);
            return 1;
        }

        // Perform the HTTP PUT request
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }

        // Clean up
        curl_easy_cleanup(curl);
    }

    // Cleanup global libcurl state
    curl_global_cleanup();

    return 0;
}

int main10(void) {
    CURL* curl;
    CURLcode res;
    char* url = "https://example.com/file.txt";
    char* filename = "file.txt";

    // Initialize libcurl
    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    if (curl) {
        // Set the URL to download from
        curl_easy_setopt(curl, CURLOPT_URL, url);

        // Set the file to save the downloaded data to
        FILE* fp = fopen(filename, "wb");
        if (fp) {
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);

            // Perform the HTTP GET request and download the file
            res = curl_easy_perform(curl);
            if (res != CURLE_OK) {
                fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            }

            // Close the file
            fclose(fp);
        }
        else {
            fprintf(stderr, "Failed to open file %s\n", filename);
            return 1;
        }

        // Clean up
        curl_easy_cleanup(curl);
    }

    // Cleanup global libcurl state
    curl_global_cleanup();

    return 0;
}