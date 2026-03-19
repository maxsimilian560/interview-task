#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <bit>
struct ListNode { // ListNode модифицировать нельзя
  ListNode* prev = nullptr; // указатель на предыдущий элемент или nullptr
  ListNode* next = nullptr;
  ListNode* rand = nullptr; // указатель на произвольный элемент данного списка, либо `nullptr` 
  std::string data; // произвольные пользовательские данные 
};
//Я бы добавил переменную rand_index, или заменил бы rand на неё, но раз модифицировать нельзя...
class List {
  ListNode* head = nullptr;
  ListNode* last = nullptr;
  uint64_t count = 0;
public:
  List() {
    //Могу использовать new если необходимо, просто я обычно работаю с malloc/calloc/realloc/free
    head = (ListNode*)calloc(1, sizeof(ListNode));
    last = head;
  }
  void push(std::string data, ListNode* rand = nullptr) {
    last->data = data;
    last->rand = rand;
    last->next = (ListNode*)calloc(1, sizeof(ListNode));
    last->next->prev = last;
    last = last->next;
    count++;
  }
  void push(const ListNode& in) {
    last->data = in.data;
    last->rand = in.rand;
    last->next = (ListNode*)calloc(1, sizeof(ListNode));
    last->next->prev = last;
    last = last->next;
    count++;
  }
  uint64_t size() {
    return count;
  }
  ListNode* get(int64_t index) {
    if (index < 0) return nullptr;
    if (index >= count) throw std::out_of_range("Index out of range '" + std::to_string(index) + "'/'" + std::to_string(count) + "'");
    ListNode* curr = head;
    for (uint64_t i = 0; i < index; i++) {
      curr = curr->next;
    }
    return curr;
  }
  int64_t get_index(ListNode* in) {
    ListNode* curr = head;
    if (in == nullptr) return -1;
    for (uint64_t i = 0; i < count; i++) {
      if (curr == in) return i;
      curr = curr->next;
    }
    throw std::out_of_range("Node '" + std::to_string((int64_t)in) + "' not found");
    //Обычно я не так часто использую throw, а использую свою систему логирования из своего фреймворка.
    //Но думаю для тестового задания это будет лишним
  }
  void print() {
    ListNode* curr = head;
    uint64_t index = 0;
    uint64_t temp;
    while (curr->next != nullptr) {
      std::cout << "- Узел " << index++ << ": \"" << curr->data << "\"" << " -> rand на узел " << ((temp = get_index(curr->rand)) == -1 ? "nullptr" : std::to_string(temp)) << std::endl;
      curr = curr->next;
    }
  }
  void remove(uint64_t index) {
    if (index >= count) throw std::out_of_range("Index out of range");
    ListNode* curr = head;
    for (uint64_t i = 0; i < index; i++) {
      curr = curr->next;
    }
    curr->prev->next = curr->next;
    curr->next->prev = curr->prev;
    curr->data.~basic_string();
    free(curr);
    count--;
  }
  void clear() {
    if (head == nullptr) return;
    ListNode* curr = head;
    while (curr != nullptr) {
      ListNode* next = curr->next;
      curr->data.~basic_string();
      free(curr);
      curr = next;
    }
  }
  ~List() {
    clear();
  }
  void save_to_file(std::string path) {
    std::ofstream file(path, std::ios::binary);
    file.write("taskmagic\n", 10);
    file.write(reinterpret_cast<const char*>(&count), sizeof(count));
    ListNode* curr = head;
    for (uint64_t i = 0; i < count; ++i) {
      uint64_t len = curr->data.size();
      int64_t index_rand = get_index(curr->rand);
      if constexpr (std::endian::native == std::endian::big) {
        char* _len = reinterpret_cast<char*>(&len);
        char* _index_rand = reinterpret_cast<char*>(&index_rand);
        std::reverse(_len, _len + sizeof(uint64_t));
        std::reverse(_index_rand, _index_rand + sizeof(int64_t));
      }

      file.write(reinterpret_cast<const char*>(&len), sizeof(len));
      file.write(curr->data.c_str(), len);
      file.write(reinterpret_cast<const char*>(&index_rand), sizeof(index_rand));
      curr = curr->next;
    }
    std::cout<<"Save to file: "<<path<<std::endl;
    file.close();
  }
  int load_from_file(std::string path) {
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) return 1;
    char magic[10];
    file.read(magic, 10);
    if (!file || memcmp(magic, "taskmagic\n", 10) != 0)
      return 1;
    uint64_t file_count;
    file.read(reinterpret_cast<char*>(&file_count), sizeof(file_count));
    if (!file) throw std::runtime_error("Read error");
    clear();
    head = (ListNode*)calloc(1, sizeof(ListNode));
    last = head;
    count = 0;
    std::vector<int64_t> temp_rand;
    temp_rand.reserve(file_count);
    for (uint64_t i = 0; i < file_count; i++) {
      uint64_t len;
      file.read(reinterpret_cast<char*>(&len), sizeof(len));
      if constexpr (std::endian::native == std::endian::big) {
        char* _len = reinterpret_cast<char*>(&len);
        std::reverse(_len, _len + sizeof(uint64_t));
      }
      std::string data(len, '\0');
      file.read(&data[0], len);

      int64_t index_rand;
      file.read(reinterpret_cast<char*>(&index_rand), sizeof(index_rand));
      if constexpr (std::endian::native == std::endian::big) {
        char* _index_rand = reinterpret_cast<char*>(&index_rand);
        std::reverse(_index_rand, _index_rand + sizeof(int64_t));
      }
      temp_rand.push_back(index_rand);
      this->push(data, nullptr);
    }
    ListNode* curr = head;
    for (uint64_t i = 0; i < file_count; i++) {
      const int64_t& lnk = temp_rand[i];
      curr->rand = (lnk == -1) ? nullptr : get(lnk);
      curr = curr->next;
    }
    if (count > 0) {
      last->prev = get(count - 1);
    }
    else {
      last->prev = nullptr;
    }
    std::cout<<"Load from file: "<<path<<std::endl;
    return 0;
  }
};
int main(int argc, char* argv[]) {
  std::string infile = "inlet.in";
  if (argc > 1) {
    if (std::string(argv[1]) == "-h" || std::string(argv[1]) == "--help") {
      std::cout << \
        R"(Usage: ./test [OPTIONS] [infile] [outfile]

Description:
  Parses a text file into a custom linked list structure.
  Supports serialization/deserialization to binary format.

Arguments:
  infile   Path to input text file (default: inlet.in)
  outfile  Path to output binary file (default: outlet.out)

Options:
  -h, --help    Show this help message and exit

Examples:
  ./test                      # Uses default inlet.in
  ./test data.txt             # Read from data.txt
  ./test data.bin save.bin    # Read + save to binary
)" << std::endl;
      return 0;
    }
    infile = argv[1];
  }
  std::string outfile = "outlet.out";
  if (argc > 2) {
    outfile = argv[2];
  }
  List list;
  if (list.load_from_file(infile)) {

    std::ifstream in(infile, std::ios::ate);
    if (!in.is_open()) { std::cerr << "Error opening file '" << infile << "'" << std::endl;return 1; }
    uint64_t size = in.tellg();
    if (size == -1) {
      std::cerr << "File is empty" << std::endl;
      return 1;
    }
    in.seekg(0, std::ios::beg);
    std::string text;
    text.resize(size);
    in.read(text.data(), size);
    ListNode temp;
    uint64_t start = 0;
    uint64_t end = text.find(';', start);
    while (end != std::string::npos) {
      uint64_t data_len = end - start;
      temp.data.assign(text.data() + start, data_len);
      start = end + 1;
      end = text.find('\n', start);
      if (end == std::string::npos) {
        end = text.size();
      }
      if (end <= start)continue;
      temp.rand = reinterpret_cast<ListNode*>(std::stoi(text.substr(start, end - start)));
      list.push(temp);
      start = end + 1;
      end = text.find(';', start);
      if (end == std::string::npos) {
        break;
      }
    }
    //Для того, чтоб изменить индексы на указатели в rand
    for (uint64_t i = 0; i < list.size(); i++) {
      list.get(i)->rand = list.get((int64_t)list.get(i)->rand);
    }
  }
  list.print();
  list.save_to_file(outfile);
  return 0;
}