#include <QCoreApplication>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <vector>
#include <filesystem>
#include <thread>

using namespace std;
namespace fs = std::filesystem;

static tuple<fs::path, fs::file_status, size_t> file_info(const fs::directory_entry &entry) // zamiast struktury zastosowałem "krotę"
{
    const auto fs (status(entry));
    return {entry.path(), fs, is_regular_file(fs) ? file_size(entry.path()) : 0u};

}

    static char type_char(fs::file_status fs)
    {
        if      (is_directory(fs))
            { return 'D'; }
        else if (is_symlink(fs))
            { return 'L'; }
        else if (is_character_file(fs))
            { return 'C'; }
        else if (is_block_file(fs))
            { return 'B'; }
        else if (is_fifo(fs))
            { return 'P'; }
        else if (is_socket(fs))
            { return 'S'; }
        else if (is_other(fs))
            { return 'O'; }
        else if (is_regular_file(fs))
            { return 'F'; }

        return '?';
    }

static string rwx(fs::perms p)
{
    auto check ([p](fs::perms bit, char c) { return (p & bit) == fs::perms::none ? '-' : c; });

    return {check(fs::perms::owner_read,   'r'),
            check(fs::perms::owner_write,  'w'),
            check(fs::perms::owner_exec,   'x'),
            check(fs::perms::group_read,   'r'),
            check(fs::perms::group_write,  'w'),
            check(fs::perms::group_exec,   'x'),
            check(fs::perms::others_read,  'r'),
            check(fs::perms::others_write, 'w'),
            check(fs::perms::others_exec,  'x')};
}

static string size_string(size_t size)
{
    stringstream ss;

    if      (size >= 1000000000)
        { ss << (size / 1000000000) << 'G'; }
    else if (size >= 1000000)
        { ss << (size / 1000000)    << 'M'; }
    else if (size >= 1000)
        { ss << (size / 1000)       << 'K'; }
    else if (size > 1 && size < 1000)
        { ss << size                << 'B'; }
    else if (size == 0)
        { ss << '-' << '-'; }
    else { ss << size << '?'; }

    return ss.str();
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);




    fs::path dir = {argc > 1 ? argv[1] : "."};

        if (!exists(dir))
        {
            cout << "Ścieżka dostępu " << dir << " nie istnieje.\n";
            return 1;
        }

        else
        {
            vector<tuple<fs::path, fs::file_status, size_t>> items;

            transform(fs::directory_iterator{dir}, {}, back_inserter(items), file_info);

            for (const auto &[path, status, size]: items)
            {

                cout << "Typ " << type_char(status) << " Uprawnienia " << rwx(status.permissions()) << " Rozmiar "
                    << setw(4) << right << size_string(size) << " Nazwa "
                    << path.filename().string() << '\n';


            }


        }
    return a.exec();
}
