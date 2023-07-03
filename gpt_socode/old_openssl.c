#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/sha.h>
#include <openssl/rand.h>
#include <openssl/evp.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/aes.h>


#define KEY_LENGTH 32
#define IV_LENGTH 16
#define KEY_SIZE 2048
#define MESSAGE_SIZE 256
#define SIGNATURE_SIZE 256

int sha256_hash(const unsigned char* message, size_t message_len, unsigned char* hash)
{
    SHA256_CTX ctx;
    int success = 0;

    // Initialize the SHA256 context
    if (SHA256_Init(&ctx) != 1)
    {
        fprintf(stderr, "Error initializing SHA256 context\n");
        return 1;
    }

    // Update the context with the message
    if (SHA256_Update(&ctx, message, message_len) != 1)
    {
        fprintf(stderr, "Error updating SHA256 context\n");
        return 1;
    }

    // Finalize the hash and store it in the provided buffer
    if (SHA256_Final(hash, &ctx) != 1)
    {
        fprintf(stderr, "Error finalizing SHA256 context\n");
        return 1;
    }

    return 0;
}

int generate_random_number(unsigned char* buffer, size_t length)
{
    if (RAND_bytes(buffer, length) != 1)
    {
        fprintf(stderr, "Error generating random number\n");
        return 1;
    }

    return 0;
}

int encrypt_data(unsigned char* plaintext, size_t plaintext_len, unsigned char* key,
    unsigned char* iv, unsigned char* ciphertext)
{
    EVP_CIPHER_CTX* ctx;
    int len, ciphertext_len;

    // Create and initialize the context
    if ((ctx = EVP_CIPHER_CTX_new()) == NULL)
    {
        fprintf(stderr, "Error creating context\n");
        return 1;
    }

    // Initialize the encryption operation with the key and IV
    if (EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv) != 1)
    {
        fprintf(stderr, "Error initializing encryption operation\n");
        return 1;
    }

    // Encrypt the plaintext
    if (EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len) != 1)
    {
        fprintf(stderr, "Error encrypting plaintext\n");
        return 1;
    }
    ciphertext_len = len;

    // Finalize the encryption operation
    if (EVP_EncryptFinal_ex(ctx, ciphertext + len, &len) != 1)
    {
        fprintf(stderr, "Error finalizing encryption operation\n");
        return 1;
    }
    ciphertext_len += len;

    // Clean up the context
    EVP_CIPHER_CTX_free(ctx);

    return ciphertext_len;
}

int decrypt_data(unsigned char* ciphertext, size_t ciphertext_len, unsigned char* key,
    unsigned char* iv, unsigned char* plaintext)
{
    EVP_CIPHER_CTX* ctx;
    int len, plaintext_len;

    // Create and initialize the context
    if ((ctx = EVP_CIPHER_CTX_new()) == NULL)
    {
        fprintf(stderr, "Error creating context\n");
        return 1;
    }

    // Initialize the decryption operation with the key and IV
    if (EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv) != 1)
    {
        fprintf(stderr, "Error initializing decryption operation\n");
        return 1;
    }

    // Decrypt the ciphertext
    if (EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len) != 1)
    {
        fprintf(stderr, "Error decrypting ciphertext\n");
        return 1;
    }
    plaintext_len = len;

    // Finalize the decryption operation
    if (EVP_DecryptFinal_ex(ctx, plaintext + len, &len) != 1)
    {
        fprintf(stderr, "Error finalizing decryption operation\n");
        return 1;
    }
    plaintext_len += len;

    // Clean up the context
    EVP_CIPHER_CTX_free(ctx);

    return plaintext_len;
}

int generate_random_number2(int* output)
{
    unsigned char buffer[sizeof(int)];

    // Generate a random number and store it in the buffer
    if (RAND_bytes(buffer, sizeof(buffer)) != 1)
    {
        fprintf(stderr, "Error generating random number\n");
        return 1;
    }

    // Copy the buffer into the output integer
    memcpy(output, buffer, sizeof(int));

    return 0;
}

int generate_key_pair(char* private_key_filename, char* public_key_filename)
{
    RSA* keypair = RSA_new();
    BIGNUM* exponent = BN_new();
    FILE* private_key_file = NULL;
    FILE* public_key_file = NULL;
    int success = 0;

    // Set the public exponent to 65537
    if (BN_set_word(exponent, 65537) != 1)
    {
        fprintf(stderr, "Error setting exponent\n");
        goto cleanup;
    }

    // Generate the RSA key pair
    if (RSA_generate_key_ex(keypair, KEY_SIZE, exponent, NULL) != 1)
    {
        fprintf(stderr, "Error generating RSA key pair\n");
        goto cleanup;
    }

    // Write the private key to a file
    private_key_file = fopen(private_key_filename, "w");
    if (private_key_file == NULL)
    {
        fprintf(stderr, "Error opening private key file\n");
        goto cleanup;
    }

    if (PEM_write_RSAPrivateKey(private_key_file, keypair, NULL, NULL, 0, NULL, NULL) != 1)
    {
        fprintf(stderr, "Error writing private key\n");
        goto cleanup;
    }

    // Write the public key to a file
    public_key_file = fopen(public_key_filename, "w");
    if (public_key_file == NULL)
    {
        fprintf(stderr, "Error opening public key file\n");
        goto cleanup;
    }

    if (PEM_write_RSAPublicKey(public_key_file, keypair) != 1)
    {
        fprintf(stderr, "Error writing public key\n");
        goto cleanup;
    }

    success = 1;

cleanup:
    if (private_key_file != NULL) fclose(private_key_file);
    if (public_key_file != NULL) fclose(public_key_file);
    if (keypair != NULL) RSA_free(keypair);
    if (exponent != NULL) BN_free(exponent);

    return success ? 0 : 1;
}

int sign_message(char* message, int message_len, char* private_key_filename, char* signature)
{
    RSA* private_key = NULL;
    FILE* private_key_file = NULL;
    unsigned char hash[SHA256_DIGEST_LENGTH];
    unsigned int signature_len = 0;
    int success = 0;

    // Load the private key from file
    private_key_file = fopen(private_key_filename, "r");
    if (private_key_file == NULL)
    {
        fprintf(stderr, "Error opening private key file\n");
        goto cleanup;
    }

    private_key = PEM_read_RSAPrivateKey(private_key_file, NULL, NULL, NULL);
    if (private_key == NULL)
    {
        fprintf(stderr, "Error loading private key\n");
        goto cleanup;
    }

    // Compute the SHA-256 hash of the message
    SHA256((unsigned char*)message, message_len, hash);

    // Sign the hash with the private key
    if (RSA_sign(NID_sha256, hash, sizeof(hash), (unsigned char*)signature, &signature_len, private_key) != 1)
    {
        fprintf(stderr, "Error signing message\n");
        goto cleanup;
    }

    success = 1;

cleanup:
    if (private_key_file != NULL) fclose(private_key_file);
    if (private_key != NULL) RSA_free(private_key);

    return success ? signature_len : -1;
}

int generate_rsa_keypair(const char* privkey_file, const char* pubkey_file)
{
    RSA* rsa;
    FILE* privkey_fp, * pubkey_fp;

    // Generate RSA key pair
    rsa = RSA_generate_key(2048, RSA_F4, NULL, NULL);
    if (!rsa) {
        fprintf(stderr, "Error generating RSA key pair\n");
        return 1;
    }

    // Save private key to file
    privkey_fp = fopen(privkey_file, "w");
    if (!privkey_fp) {
        fprintf(stderr, "Error opening private key file\n");
        RSA_free(rsa);
        return 1;
    }
    if (!PEM_write_RSAPrivateKey(privkey_fp, rsa, NULL, NULL, 0, NULL, NULL)) {
        fprintf(stderr, "Error writing private key to file\n");
        RSA_free(rsa);
        fclose(privkey_fp);
        return 1;
    }
    fclose(privkey_fp);

    // Save public key to file
    pubkey_fp = fopen(pubkey_file, "w");
    if (!pubkey_fp) {
        fprintf(stderr, "Error opening public key file\n");
        RSA_free(rsa);
        return 1;
    }
    if (!PEM_write_RSAPublicKey(pubkey_fp, rsa)) {
        fprintf(stderr, "Error writing public key to file\n");
        RSA_free(rsa);
        fclose(pubkey_fp);
        return 1;
    }
    fclose(pubkey_fp);

    RSA_free(rsa);
    return 0;
}

void encrypt_message(const unsigned char* message, const unsigned char* key, unsigned char* ciphertext)
{
    AES_KEY aes_key;
    unsigned char iv[AES_BLOCK_SIZE];
    int message_len = strlen((char*)message);
    int num_blocks = (message_len + AES_BLOCK_SIZE - 1) / AES_BLOCK_SIZE;

    // Set up initialization vector
    memset(iv, 0, AES_BLOCK_SIZE);

    // Set up encryption key
    if (AES_set_encrypt_key(key, 256, &aes_key) != 0) {
        fprintf(stderr, "Error setting up AES encryption key\n");
        exit(1);
    }

    // Encrypt each block of the message using AES-CBC mode
    AES_cbc_encrypt(message, ciphertext, message_len, &aes_key, iv, AES_ENCRYPT);

    // Pad the last block of ciphertext to be the same size as the other blocks
    int last_block_size = message_len % AES_BLOCK_SIZE;
    if (last_block_size != 0) {
        int pad_len = AES_BLOCK_SIZE - last_block_size;
        memset(ciphertext + (num_blocks - 1) * AES_BLOCK_SIZE + last_block_size, pad_len, pad_len);
    }
}

void generate_random_string(char* random_string, int string_length)
{
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    int charset_length = sizeof(charset) - 1;

    for (int i = 0; i < string_length; i++) {
        unsigned char random_byte;
        if (RAND_bytes(&random_byte, 1) == 1) {
            random_string[i] = charset[random_byte % charset_length];
        }
        else {
            fprintf(stderr, "Error generating random byte\n");
            exit(1);
        }
    }

    random_string[string_length] = '\0';
}

