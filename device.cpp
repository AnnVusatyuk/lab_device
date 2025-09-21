#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <cmath>

using namespace std;

int streamcounter; ///< Global variable to keep track of stream creation.

const int MIXER_OUTPUTS = 1;
const float POSSIBLE_ERROR = 0.01;

class Stream
{
private:
    double mass_flow; ///< The mass flow rate of the stream.
    string name;      ///< The name of the stream.

public:
    Stream(int s){setName("s"+std::to_string(s));}

    void setName(string s){name=s;}

    string getName(){return name;}

    void setMassFlow(double m){mass_flow=m;}

    double getMassFlow() const {return mass_flow;}

    void print() { cout << "Stream " << getName() << " flow = " << getMassFlow() << endl; }
};

class Device
{
protected:
    vector<shared_ptr<Stream>> inputs;  ///< Input streams connected to the device.
    vector<shared_ptr<Stream>> outputs; ///< Output streams produced by the device.
    int inputAmount;
    int outputAmount;
public:
    void addInput(shared_ptr<Stream> s){
      if(inputs.size() < inputAmount) inputs.push_back(s);
      else throw"INPUT STREAM LIMIT!";
    }

    void addOutput(shared_ptr<Stream> s){
      if(outputs.size() < outputAmount) outputs.push_back(s);
      else throw "OUTPUT STREAM LIMIT!";
    }

    virtual void updateOutputs() = 0;
};

/**
 * @class Mixer
 * @brief Моделирует работу смесителя с N входов и 1 выходом
 */
class Mixer: public Device
{
    private:
      int _inputs_count = 0;
    public:
      /**
       * @brief Конструктор смесителя с заданным количеством входных потоков
       * @param inputs_count Количество входных потоков
       */
      Mixer(int inputs_count): Device() {
        _inputs_count = inputs_count;
      }

      /**
       * @brief Добавление входного потока
       * @param s Входной поток
       */
      void addInput(shared_ptr<Stream> s) {
        if (inputs.size() == _inputs_count) {
          throw "Too much inputs"s;
        }
        inputs.push_back(s);
      }

      /**
       * @brief Добавление выходного потока
       * @param s Выходной поток
       */
      void addOutput(shared_ptr<Stream> s) {
        if (outputs.size() == MIXER_OUTPUTS) {
          throw "Too much outputs"s;
        }
        outputs.push_back(s);
      }

      /**
       * @brief Обновление расхода на выходе. Суммируется расход всех входных потоков,
       * результат присваивается выходу
       */
      void updateOutputs() override {
        double sum_mass_flow = 0;
        for (const auto& input_stream : inputs) {
          sum_mass_flow += input_stream -> getMassFlow();
        }

        if (outputs.empty()) {
          throw "Should set outputs before update"s;
        }

        double output_mass = sum_mass_flow / outputs.size(); // divide 0

        for (auto& output_stream : outputs) {
          output_stream -> setMassFlow(output_mass);
        }
      }
};

/**
* @brief Проверка правильного суммирования входных потоков 
*/
void Test1() {
    streamcounter=0;
    Mixer d1 = Mixer(2);
    
    shared_ptr<Stream> s1(new Stream(++streamcounter));
    shared_ptr<Stream> s2(new Stream(++streamcounter));
    shared_ptr<Stream> s3(new Stream(++streamcounter));
    s1->setMassFlow(10.0);
    s2->setMassFlow(5.0);

    d1.addInput(s1);
    d1.addInput(s2);
    d1.addOutput(s3);

    d1.updateOutputs();

    if (abs(s3->getMassFlow()) - 15 < POSSIBLE_ERROR) {
      cout << "Test 1 passed"s << endl;
    } else {
      cout << "Test 1 failed"s << endl;
    }
}

/**
* @brief Попытка добавления больше 1 потока на выход
*/
void Test2() {
    streamcounter=0;
    Mixer d1 = Mixer(2);
    
    shared_ptr<Stream> s1(new Stream(++streamcounter));
    shared_ptr<Stream> s2(new Stream(++streamcounter));
    shared_ptr<Stream> s3(new Stream(++streamcounter));
    shared_ptr<Stream> s4(new Stream(++streamcounter));
    s1->setMassFlow(10.0);
    s2->setMassFlow(5.0);

    d1.addInput(s1);
    d1.addInput(s2);
    d1.addOutput(s3);

    try {
      d1.addOutput(s4);
    } catch (const string ex) {
      if (ex == "Too much outputs"s) {
        cout << "Test 2 passed"s << endl;

        return;
      }
    }

    cout << "Test 2 failed"s << endl;
}

/**
* @brief Попытка добавления большего количества потоков,
* чем было изначально задано для смесителя
*/
void Test3() {
    streamcounter=0;
    Mixer d1 = Mixer(2);
    
    shared_ptr<Stream> s1(new Stream(++streamcounter));
    shared_ptr<Stream> s2(new Stream(++streamcounter));
    shared_ptr<Stream> s3(new Stream(++streamcounter));
    shared_ptr<Stream> s4(new Stream(++streamcounter));
    s1->setMassFlow(10.0);
    s2->setMassFlow(5.0);

    d1.addInput(s1);
    d1.addInput(s2);
    d1.addOutput(s3);

    try {
      d1.addInput(s4);
    } catch (const string ex) {
      if (ex == "Too much inputs"s) {
        cout << "Test 3 passed"s << endl;

        return;
      }
    }

    cout << "Test 3 failed"s << endl;
}

/**
* @brief Проверка подачи отрицательных потоков на вход
*/
void Test4() {
    streamcounter = 0;
    Mixer d1(2);

    shared_ptr<Stream> s1(new Stream(++streamcounter));
    shared_ptr<Stream> s2(new Stream(++streamcounter));
    shared_ptr<Stream> s3(new Stream(++streamcounter));

    s1->setMassFlow(10.0);
    s2->setMassFlow(-5.0);

    d1.addInput(s1);
    d1.addInput(s2);
    d1.addOutput(s3);

    d1.updateOutputs();

    if (abs(s3->getMassFlow() - 5) < POSSIBLE_ERROR) {
        cout << "Test 5 passed" << endl;
    }
    else {
        cout << "Test 5 failed" << endl;
    }
}

/**
* @brief Проверка пустых входных потоков 
* (выход должен быть 0, если не заданы никакие входы)
*/
void Test5() {
    streamcounter = 0;
    Mixer d1(2);

    shared_ptr<Stream> s1(new Stream(++streamcounter));
    d1.addOutput(s1);

    d1.updateOutputs();

    if (abs(s1->getMassFlow()) == 0) {
        cout << "Test 4 passed" << endl;
    }
    else {
        cout << "Test 4 failed" << endl;
    }
}

/**
* @brief Проверка, что updateOutputs() корректно пересчитывает выход
* при изменении входных потоков
*/
void Test6() {
    streamcounter = 0;
    Mixer d1(2);

    shared_ptr<Stream> s1(new Stream(++streamcounter));
    shared_ptr<Stream> s2(new Stream(++streamcounter));
    shared_ptr<Stream> s3(new Stream(++streamcounter));

    s1->setMassFlow(10.0);
    s2->setMassFlow(5.0);

    d1.addInput(s1);
    d1.addInput(s2);
    d1.addOutput(s3);

    d1.updateOutputs();
    if (abs(s3->getMassFlow() - 15) > POSSIBLE_ERROR) {
        cout << "Test 6 failed" << endl;
        return;
    }

    s1->setMassFlow(20.0);
    d1.updateOutputs();

    if (abs(s3->getMassFlow() - 25) < POSSIBLE_ERROR) {
        cout << "Test 6 passed" << endl;
    }
    else {
        cout << "Test 6 failed" << endl;
    }
}

/**
* @brief Проверка отсутствия выходного потока
*/
void Test7() {
    streamcounter = 0;
    Mixer d1(2);

    shared_ptr<Stream> s1(new Stream(++streamcounter));
    shared_ptr<Stream> s2(new Stream(++streamcounter));
    s1->setMassFlow(5.0);
    s2->setMassFlow(5.0);

    d1.addInput(s1);
    d1.addInput(s2);

    try {
        d1.updateOutputs();
    }
    catch (const string& ex) {
        if (ex == "Should set outputs before update") {
            cout << "Test 7 passed" << endl;
            return;
        }
    }
    cout << "Test 7 failed" << endl;
}


void tests(){
    Test1();
    Test2();
    Test3();
    Test4();
    Test5();
    Test6();
    Test7();
}

int main()
{
    streamcounter = 0;

    shared_ptr<Stream> s1(new Stream(++streamcounter));
    shared_ptr<Stream> s2(new Stream(++streamcounter));
    shared_ptr<Stream> s3(new Stream(++streamcounter));

    s1->setMassFlow(10.0);
    s2->setMassFlow(5.0);

    Mixer d1(2);
    d1.addInput(s1);
    d1.addInput(s2);
    d1.addOutput(s3);

    d1.updateOutputs();

    s1->print();
    s2->print();
    s3->print();
    tests();

    return 0;
}
