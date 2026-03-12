#include <hal/debug.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <io.h>
#include <errno.h>
#include <xboxkrnl/xboxkrnl.h>

#define TEST_FILE "E:\\Apps\\BUILD\\default.xbe.bak2"

static int tests_passed = 0;
static int tests_failed = 0;

static void expect(int cond, const char *msg)
{
    if (cond) {
        debugPrint("[PASS] %s\n", msg);
        DbgPrint("[PASS] %s\n", msg);
        tests_passed++;
    } else {
        debugPrint("[FAIL] %s (errno=%d)\n", msg, errno);
        DbgPrint("[FAIL] %s (errno=%d)\n", msg, errno);
        tests_failed++;
    }
}

/* ---------------------------------------------------------- */

static void test_create_and_write(void)
{
    debugPrint("Running: test_create_and_write\n");

    int fd = _open(TEST_FILE, _O_CREAT | _O_TRUNC | _O_RDWR | _O_BINARY, 0644);
    expect(fd >= 0, "open _O_CREAT | _O_TRUNC");

    const char *msg = "HelloWorld";
    int w = _write(fd, msg, strlen(msg));
    expect(w == (int)strlen(msg), "write 10 bytes");

    _close(fd);
}

/* ---------------------------------------------------------- */

static void test_read_back(void)
{
    debugPrint("Running: test_read_back\n");

    int fd = _open(TEST_FILE, _O_RDONLY | _O_BINARY);
    expect(fd >= 0, "open _O_RDONLY");

    char buf[32] = {0};
    int r = _read(fd, buf, sizeof(buf));
    expect(r == 10, "read 10 bytes");
    expect(strcmp(buf, "HelloWorld") == 0, "content matches");

    _close(fd);
}

/* ---------------------------------------------------------- */

static void test_truncate(void)
{
    debugPrint("Running: test_truncate\n");

    int fd = _open(TEST_FILE, _O_TRUNC | _O_RDWR);
    expect(fd >= 0, "open _O_TRUNC");

    long size = _lseek(fd, 0, SEEK_END);
    expect(size == 0, "file truncated to 0");

    _close(fd);
}

/* ---------------------------------------------------------- */

static void test_append(void)
{
    debugPrint("Running: test_append\n");

    int fd = _open(TEST_FILE, _O_CREAT | _O_TRUNC | _O_RDWR | _O_BINARY, 0644);
    expect(fd >= 0, "open for append test");

    _write(fd, "ABC", 3);
    _close(fd);

    fd = _open(TEST_FILE, _O_APPEND | _O_RDWR | _O_BINARY);
    expect(fd >= 0, "open _O_APPEND");

    _write(fd, "DEF", 3);
    _close(fd);

    fd = _open(TEST_FILE, _O_RDONLY | _O_BINARY);
    char buf[16] = {0};
    _read(fd, buf, sizeof(buf));
    expect(strcmp(buf, "ABCDEF") == 0, "append worked");

    _close(fd);
}

/* ---------------------------------------------------------- */

static void test_lseek(void)
{
    debugPrint("Running: test_lseek\n");

    int fd = _open(TEST_FILE, _O_CREAT | _O_TRUNC | _O_RDWR | _O_BINARY, 0644);
    expect(fd >= 0, "open for lseek");

    _write(fd, "0123456789", 10);

    _lseek(fd, 5, SEEK_SET);
    _write(fd, "X", 1);

    _lseek(fd, 0, SEEK_SET);

    char buf[16] = {0};
    _read(fd, buf, sizeof(buf));

    expect(strcmp(buf, "01234X6789") == 0, "lseek overwrite");

    _close(fd);
}

/* ---------------------------------------------------------- */

static void test_text_mode_translation(void)
{
    debugPrint("Running: test_text_mode_translation\n");

    int fd = _open(TEST_FILE, _O_CREAT | _O_TRUNC | _O_RDWR | _O_TEXT, 0644);
    expect(fd >= 0, "open _O_TEXT");

    _write(fd, "A\nB\n", 4);
    _close(fd);

    fd = _open(TEST_FILE, _O_RDONLY | _O_TEXT);
    char buf[16] = {0};
    int r = _read(fd, buf, sizeof(buf));

    expect(r == 4, "text read size");
    expect(strcmp(buf, "A\nB\n") == 0, "CRLF translated");

    _close(fd);
}

/* ---------------------------------------------------------- */

static void test_error_conditions(void)
{
    debugPrint("Running: test_error_conditions\n");

    int fd = _open(TEST_FILE, _O_RDONLY);
    expect(fd >= 0, "open readonly");

    int w = _write(fd, "X", 1);
    expect(w == -1 && errno == EBADF, "write on readonly fails");

    _close(fd);

    fd = _open("D:/does_not_exist.txt", _O_RDONLY);
    expect(fd == -1, "open non-existing file fails");
}

/* ---------------------------------------------------------- */

void nxio_run_tests(void)
{
    debugPrint("\n==== NXIO TEST SUITE START ====\n\n");

    test_create_and_write();
    test_read_back();
    test_truncate();
    test_append();
    test_lseek();
    test_text_mode_translation();
    test_error_conditions();

    debugPrint("\n==== TEST RESULTS ====\n");
    debugPrint("Passed: %d\n", tests_passed);
    debugPrint("Failed: %d\n", tests_failed);
    debugPrint("=======================\n\n");
}