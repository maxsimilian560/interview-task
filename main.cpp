#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
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
  size_t count = 0;
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
  size_t size() {
    return count;
  }
  ListNode* get(long long index) {
    if (index < 0) return nullptr;
    if (index >= count) throw std::out_of_range("Index out of range '" + std::to_string(index) + "'/'" + std::to_string(count) + "'");
    ListNode* curr = head;
    for (size_t i = 0; i < index; i++) {
      curr = curr->next;
    }
    return curr;
  }
  long long get_index(ListNode* in) {
    ListNode* curr = head;
    if (in == nullptr) return -1;
    for (size_t i = 0; i < count; i++) {
      if (curr == in) return i;
      curr = curr->next;
    }
    throw std::out_of_range("Node '" + std::to_string((long long)in) + "' not found");
    //Обычно я не так часто использую throw, а использую свою систему логирования из своего фреймворка.
    //Но думаю для тестового задания это будет лишним
  }
  void print() {
    ListNode* curr = head;
    size_t index = 0;
    size_t temp;
    while (curr->next != nullptr) {
      std::cout << "- Узел " << index++ << ": \"" << curr->data << "\"" << " -> rand на узел " << ((temp = get_index(curr->rand)) == -1 ? "nullptr" : std::to_string(temp)) << std::endl;
      curr = curr->next;
    }
  }
  void remove(size_t index) {
    if (index >= count) throw std::out_of_range("Index out of range");
    ListNode* curr = head;
    for (size_t i = 0; i < index; i++) {
      curr = curr->next;
    }
    curr->prev->next = curr->next;
    curr->next->prev = curr->prev;
    curr->data.~basic_string();
    free(curr);
    count--;
  }
  ~List() {
    if (head == nullptr) return;
    ListNode* curr = head;
    while (curr != nullptr) {
      ListNode* next = curr->next;
      curr->data.~basic_string();
      free(curr);
      curr = next;
    }
  }
};
int main(int argc, char* argv[]) {
  std::string infile = "inlet.in";
  if (argc > 1) {
    infile = argv[1];
  }
  std::ifstream in(infile, std::ios::ate);
  if (!in.is_open()) { std::cerr << "Error opening file '" << infile << "'" << std::endl;return 1; }
  size_t size = in.tellg();
  if (size == -1) {
    std::cerr << "File is empty" << std::endl;
    return 1;
  }
  in.seekg(0, std::ios::beg);
  std::string text;
  text.resize(size);
  in.read(text.data(), size);
  List list;
  ListNode temp;
  size_t start = 0;
  size_t end = text.find(';', start);
  while (end != std::string::npos) {
    size_t data_len = end - start;
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
  for (size_t i = 0; i < list.size(); i++) {
    list.get(i)->rand = list.get((long long)list.get(i)->rand);
  }
  list.print();
  return 0;
}