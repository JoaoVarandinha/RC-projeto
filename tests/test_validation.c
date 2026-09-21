// Unit tests for validation.c
// Cases encode what the PROJECT SPEC requires, not what the code currently does,
// so a failure means either the code or my reading of the spec is wrong.
//
// Build and run from the project root:
//   gcc -Wall -o tests/test_validation tests/test_validation.c validation.c && ./tests/test_validation

#include "../validation.h"

#include <stdio.h>
#include <string.h>

static int checks = 0;
static int failures = 0;

static void check(const char* function, const char* input, bool got, bool want){
    checks++;
    if (got != want){
        failures++;
        printf("  FAIL  %s(%s) returned %s, expected %s\n",
               function, input, got ? "true" : "false", want ? "true" : "false");
    }
}

static void test_is_valid_UID(){
    printf("is_valid_UID\n");
    //Exactly 6 decimal digits
    check("is_valid_UID", "\"123456\"", is_valid_UID("123456"), true);
    check("is_valid_UID", "\"000000\"", is_valid_UID("000000"), true);
    check("is_valid_UID", "\"999999\"", is_valid_UID("999999"), true);

    check("is_valid_UID", "NULL", is_valid_UID(NULL), false);
    check("is_valid_UID", "\"\"", is_valid_UID(""), false);
    check("is_valid_UID", "\"12345\"", is_valid_UID("12345"), false);
    check("is_valid_UID", "\"1234567\"", is_valid_UID("1234567"), false);
    check("is_valid_UID", "\"12345a\"", is_valid_UID("12345a"), false);
    check("is_valid_UID", "\"12 456\"", is_valid_UID("12 456"), false);
    check("is_valid_UID", "\"-12345\"", is_valid_UID("-12345"), false);
}

static void test_is_valid_password(){
    printf("is_valid_password\n");
    //Exactly 8 ASCII alphanumeric characters
    check("is_valid_password", "\"abcdefgh\"", is_valid_password("abcdefgh"), true);
    check("is_valid_password", "\"12345678\"", is_valid_password("12345678"), true);
    check("is_valid_password", "\"Pass1234\"", is_valid_password("Pass1234"), true);

    check("is_valid_password", "NULL", is_valid_password(NULL), false);
    check("is_valid_password", "\"\"", is_valid_password(""), false);
    check("is_valid_password", "\"abcdefg\"", is_valid_password("abcdefg"), false);
    check("is_valid_password", "\"abcdefghi\"", is_valid_password("abcdefghi"), false);
    check("is_valid_password", "\"abcdefg!\"", is_valid_password("abcdefg!"), false);
    check("is_valid_password", "\"abcdef h\"", is_valid_password("abcdef h"), false);
    check("is_valid_password", "\"pass_123\"", is_valid_password("pass_123"), false);
}

static void test_is_valid_PORT(){
    printf("is_valid_PORT\n");
    //Decimal integer from 1 to 65535
    check("is_valid_PORT", "\"1\"", is_valid_PORT("1"), true);
    check("is_valid_PORT", "\"80\"", is_valid_PORT("80"), true);
    check("is_valid_PORT", "\"59000\"", is_valid_PORT("59000"), true);
    check("is_valid_PORT", "\"65535\"", is_valid_PORT("65535"), true);

    check("is_valid_PORT", "NULL", is_valid_PORT(NULL), false);
    check("is_valid_PORT", "\"\"", is_valid_PORT(""), false);
    check("is_valid_PORT", "\"0\"", is_valid_PORT("0"), false);
    check("is_valid_PORT", "\"65536\"", is_valid_PORT("65536"), false);
    check("is_valid_PORT", "\"99999\"", is_valid_PORT("99999"), false);
    check("is_valid_PORT", "\"123456\"", is_valid_PORT("123456"), false);
    check("is_valid_PORT", "\"-1\"", is_valid_PORT("-1"), false);
    check("is_valid_PORT", "\"12a\"", is_valid_PORT("12a"), false);
    check("is_valid_PORT", "\"abc\"", is_valid_PORT("abc"), false);
}

static void test_is_valid_IP(){
    printf("is_valid_IP\n");
    check("is_valid_IP", "\"192.168.1.1\"", is_valid_IP("192.168.1.1"), true);
    check("is_valid_IP", "\"193.136.138.142\"", is_valid_IP("193.136.138.142"), true);
    check("is_valid_IP", "\"0.0.0.0\"", is_valid_IP("0.0.0.0"), true);
    check("is_valid_IP", "\"255.255.255.255\"", is_valid_IP("255.255.255.255"), true);

    check("is_valid_IP", "NULL", is_valid_IP(NULL), false);
    check("is_valid_IP", "\"\"", is_valid_IP(""), false);
    check("is_valid_IP", "\"256.1.1.1\"", is_valid_IP("256.1.1.1"), false);
    check("is_valid_IP", "\"1.2.3\"", is_valid_IP("1.2.3"), false);
    check("is_valid_IP", "\"1.2.3.4.5\"", is_valid_IP("1.2.3.4.5"), false);
    check("is_valid_IP", "\"abc\"", is_valid_IP("abc"), false);
}

static void test_is_valid_filename(){
    printf("is_valid_filename\n");
    //Max 24 chars total; base of letters, digits, - and _; dot; exactly 3 alphanumerics
    check("is_valid_filename", "\"a.txt\"", is_valid_filename("a.txt"), true);
    check("is_valid_filename", "\"notes.txt\"", is_valid_filename("notes.txt"), true);
    check("is_valid_filename", "\"video.mp4\"", is_valid_filename("video.mp4"), true);
    check("is_valid_filename", "\"my_file-v2.mp3\"", is_valid_filename("my_file-v2.mp3"), true);
    //20 char base + dot + 3 char extension = 24, the maximum
    check("is_valid_filename", "24 chars", is_valid_filename("abcdefghijabcdefghij.txt"), true);

    check("is_valid_filename", "NULL", is_valid_filename(NULL), false);
    check("is_valid_filename", "\"\"", is_valid_filename(""), false);
    //25 chars, one over the limit
    check("is_valid_filename", "25 chars", is_valid_filename("abcdefghijabcdefghijk.txt"), false);
    check("is_valid_filename", "\"noextension\"", is_valid_filename("noextension"), false);
    check("is_valid_filename", "\"file.tx\"", is_valid_filename("file.tx"), false);
    check("is_valid_filename", "\"file.text\"", is_valid_filename("file.text"), false);
    check("is_valid_filename", "\"file.t_t\"", is_valid_filename("file.t_t"), false);
    check("is_valid_filename", "\"my.file.txt\"", is_valid_filename("my.file.txt"), false);
    check("is_valid_filename", "\"file name.txt\"", is_valid_filename("file name.txt"), false);
    check("is_valid_filename", "\"dir/file.txt\"", is_valid_filename("dir/file.txt"), false);
    check("is_valid_filename", "\".txt\"", is_valid_filename(".txt"), false);
}

static void test_is_valid_label(){
    printf("is_valid_label\n");
    //1 to 20 characters: letters, digits, - and _
    check("is_valid_label", "\"a\"", is_valid_label("a"), true);
    check("is_valid_label", "\"HD\"", is_valid_label("HD"), true);
    check("is_valid_label", "\"480p\"", is_valid_label("480p"), true);
    check("is_valid_label", "\"1080p\"", is_valid_label("1080p"), true);
    check("is_valid_label", "\"high_quality-v2\"", is_valid_label("high_quality-v2"), true);
    check("is_valid_label", "20 chars", is_valid_label("abcdefghijabcdefghij"), true);

    check("is_valid_label", "NULL", is_valid_label(NULL), false);
    check("is_valid_label", "\"\"", is_valid_label(""), false);
    check("is_valid_label", "21 chars", is_valid_label("abcdefghijabcdefghijk"), false);
    check("is_valid_label", "\"has space\"", is_valid_label("has space"), false);
    check("is_valid_label", "\"bad!\"", is_valid_label("bad!"), false);
    check("is_valid_label", "\"dot.ted\"", is_valid_label("dot.ted"), false);
}

static void test_isLoggedIn(){
    printf("isLoggedIn\n");
    user_info empty;
    empty.UID[0] = '\0';
    empty.password[0] = '\0';
    check("isLoggedIn", "empty session", isLoggedIn(empty), false);

    user_info full;
    strcpy(full.UID, "123456");
    strcpy(full.password, "abcdefgh");
    check("isLoggedIn", "full session", isLoggedIn(full), true);

    //A session cleared by logout blanks both fields
    user_info cleared = full;
    cleared.UID[0] = '\0';
    cleared.password[0] = '\0';
    check("isLoggedIn", "cleared session", isLoggedIn(cleared), false);
}

int main(){
    test_is_valid_UID();
    test_is_valid_password();
    test_is_valid_PORT();
    test_is_valid_IP();
    test_is_valid_filename();
    test_is_valid_label();
    test_isLoggedIn();

    printf("\n%d checks, %d failures\n", checks, failures);
    return failures == 0 ? 0 : 1;
}
