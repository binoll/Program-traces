/**
 * @file regf_analyzer.cpp
 * @brief Анализатор файлов реестра Windows с использованием libregf.
 * @author Ваше Имя
 * @date 2023-11-20
 * @warning Требуется установленная библиотека libregf!
 */

#include <libregf.h>
#include <iostream>
#include <memory>
#include <system_error>

using namespace std;

// Умные указатели для автоматического освобождения ресурсов libregf
using RegfFilePtr = unique_ptr<libregf_file_t, void(*)(libregf_file_t*)>;
using RegfKeyPtr = unique_ptr<libregf_key_t, void(*)(libregf_key_t*)>;

/**
 * @brief Открывает файл реестра и возвращает объект файла.
 * @param filename Путь к файлу реестра (например, NTUSER.DAT).
 * @return Указатель на файл реестра.
 * @throw runtime_error В случае ошибки.
 */
RegfFilePtr open_regf_file(const string& filename) {
  libregf_file_t* regf_file = nullptr;
  int result = libregf_file_initialize(&regf_file, nullptr);

  if (result != 1 || !regf_file) {
    throw runtime_error("Ошибка инициализации объекта файла!");
  }

  result = libregf_file_open(regf_file, filename.c_str(), LIBREGF_OPEN_READ);
  if (result != 1) {
    libregf_file_free(&regf_file, nullptr);
    throw runtime_error("Не удалось открыть файл реестра!");
  }

  return RegfFilePtr(regf_file, libregf_file_free);
}

/**
 * @brief Выводит информацию о ключе реестра и его значениях.
 * @param key Указатель на ключ реестра.
 */
void print_key_info(libregf_key_t* key) {
  const char* key_name = nullptr;
  libregf_key_get_name(key, &key_name, nullptr);

  cout << "Ключ: " << (key_name ? key_name : "N/A") << endl;

  // Получение значений ключа
  int num_values = 0;
  libregf_key_get_number_of_values(key, &num_values, nullptr);

  cout << "Количество значений: " << num_values << endl << endl;
}

/**
 * @brief Рекурсивный обход подключачей реестра.
 * @param key Указатель на текущий ключ.
 * @param depth Глубина вложенности (для отступов).
 */
void traverse_registry_keys(libregf_key_t* key, int depth = 0) {
  if (!key) return;

  // Вывод информации о текущем ключе
  print_key_info(key);

  // Обход подключачей
  libregf_key_t* subkey = nullptr;
  int num_subkeys = 0;
  libregf_key_get_number_of_sub_keys(key, &num_subkeys, nullptr);

  for (int i = 0; i < num_subkeys; ++i) {
    if (libregf_key_get_sub_key(key, i, &subkey, nullptr) == 1) {
      RegfKeyPtr subkey_ptr(subkey, libregf_key_free);
      traverse_registry_keys(subkey_ptr.get(), depth + 1);
    }
  }
}

int main(int argc, char** argv) {
  if (argc != 2) {
    cerr << "Использование: " << argv[0] << " <файл_реестра>" << endl;
    return 1;
  }

  try {
    auto regf_file = open_regf_file(argv[1]);

    // Получение корневого ключа
    libregf_key_t* root_key = nullptr;
    if (libregf_file_get_root_key(regf_file.get(), &root_key, nullptr) != 1) {
      throw runtime_error("Не удалось получить корневой ключ!");
    }
    RegfKeyPtr root_key_ptr(root_key, libregf_key_free);

    cout << "Начало анализа файла: " << argv[1] << endl << endl;
    traverse_registry_keys(root_key_ptr.get());

  } catch (const exception& e) {
    cerr << "Ошибка: " << e.what() << endl;
    return 1;
  }

  return 0;
}