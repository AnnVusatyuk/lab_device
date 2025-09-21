/**
 * @file main.cpp
 *
 * @brief A C++ program demonstrating the usage of the Stream and Device classes.
 */

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <cmath>

using namespace std;

int streamcounter; ///< Global variable to keep track of stream creation.

const int MIXER_OUTPUTS = 1;
const float POSSIBLE_ERROR = 0.01;

/**
 * @class Stream
 * @brief Represents a chemical stream with a name and mass flow.
 */
class Stream
{
private:
    double mass_flow; ///< The mass flow rate of the stream.
    string name;      ///< The name of the stream.

public:
    /**
     * @brief Constructor to create a Stream with a unique name.
     * @param s An integer used to generate a unique name for the stream.
     */
    Stream(int s){setName("s"+std::to_string(s));}

    /**
     * @brief Set the name of the stream.
     * @param s The new name for the stream.
     */
    void setName(string s){name=s;}

    /**
     * @brief Get the name of the stream.
     * @return The name of the stream.
     */
    string getName(){return name;}

    /**
     * @brief Set the mass flow rate of the stream.
     * @param m The new mass flow rate value.
     */
    void setMassFlow(double m){mass_flow=m;}

    /**
     * @brief Get the mass flow rate of the stream.
     * @return The mass flow rate of the stream.
     */
    double getMassFlow() const {return mass_flow;}

    /**
     * @brief Print information about the stream.
     */
    void print() { cout << "Stream " << getName() << " flow = " << getMassFlow() << endl; }
};

/**
 * @class Device
 * @brief Represents a device that manipulates chemical streams.
 */
class Device
{
protected:
    vector<shared_ptr<Stream>> inputs;  ///< Input streams connected to the device.
    vector<shared_ptr<Stream>> outputs; ///< Output streams produced by the device.
    int inputAmount;
    int outputAmount;
public:
    /**
     * @brief Add an input stream to the device.
     * @param s A shared pointer to the input stream.
     */
    void addInput(shared_ptr<Stream> s){
      if(inputs.size() < inputAmount) inputs.push_back(s);
      else throw"INPUT STREAM LIMIT!";
    }
    /**
     * @brief Add an output stream to the device.
     * @param s A shared pointer to the output stream.
     */
    void addOutput(shared_ptr<Stream> s){
      if(outputs.size() < outputAmount) outputs.push_back(s);
      else throw "OUTPUT STREAM LIMIT!";
    }

    /**
     * @brief Update the output streams of the device (to be implemented by derived classes).
     */
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

/**
 * @brief The entry point of the program.
 * @return 0 on successful execution.
 */
int main()
{
    streamcounter = 0;

    // Create streams
    shared_ptr<Stream> s1(new Stream(++streamcounter));
    shared_ptr<Stream> s2(new Stream(++streamcounter));
    shared_ptr<Stream> s3(new Stream(++streamcounter));

    // Set mass flows
    s1->setMassFlow(10.0);
    s2->setMassFlow(5.0);

    // Create a device (Mixer) and add input/output streams
    Mixer d1(2);
    d1.addInput(s1);
    d1.addInput(s2);
    d1.addOutput(s3);

    // Update the outputs of the device
    d1.updateOutputs();

    // Print stream information
    s1->print();
    s2->print();
    s3->print();
    tests();

    return 0;
}
