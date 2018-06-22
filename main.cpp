#include <iostream>
#include <vector>
#include <list>
#include <iterator>
#include <bits/stdc++.h>

using namespace std;

struct simple_neuron
{
    enum state {LOWER_STATE=-1, UPPER_STATE=1};

    typedef float coefficient;
    typedef state state_t;

    static state read(char c)
    {
        return (c == '*' ? UPPER_STATE : LOWER_STATE);
    }

    static char write(state s)
    {
        if (s == LOWER_STATE)
        {
            return ' ';
        }
        else
        {
            return '*';
        }
    }

    template <typename _Iv, typename _Ic>
    static state_t calculate(_Iv val_b, _Iv val_e, _Ic coeff_b)
    {
        auto value = std::inner_product(
                         val_b,
                         val_e,
                         coeff_b,
                         coefficient(0)
                     );

        return value > 0 ? UPPER_STATE : LOWER_STATE;
    }
};

typedef simple_neuron neuron_type;
typedef neuron_type::state_t state_t;
typedef vector<state_t> neurons_line;
typedef vector<vector<neuron_type::coefficient>> link_coeffs;


struct neuro_net_system
{
    const link_coeffs &_coeffs;

    neuro_net_system(const link_coeffs &coeffs): _coeffs(coeffs) {}

    bool do_step(neurons_line& line)
    {
        bool value_changed = false;

        neurons_line old_values(begin(line), end(line));
        link_coeffs::const_iterator it_coeffs = begin(_coeffs);

        std::transform(
            begin(line),
            end(line),
            begin(line),
            [&old_values, &it_coeffs, &value_changed] (state_t old_value) -> state_t{
            auto new_value = neuron_type::calculate(
                begin(old_values),
                end(old_values),
                begin(*it_coeffs++)
            );

            value_changed = (new_value != old_value) || value_changed;

            return new_value;
        });

        return value_changed;
    }

    size_t _do(neurons_line& line)
    {
        bool end_marker = true;
        _steps_done = 0;

        while (end_marker)
        {
            end_marker = do_step(line);
            ++_steps_done;
        }

        return _steps_done;
    }

    size_t steps_done() const
    {
        return _steps_done;
    }
private:
    size_t _steps_done;
};

link_coeffs learn_neuro_net(const list<neurons_line> &src_images)
{
    link_coeffs result_coeffs;
    size_t neurons_count = src_images.front().size();

    result_coeffs.resize(neurons_count);
    for (size_t i = 0; i < neurons_count; ++i)
    {
        result_coeffs[i].resize(neurons_count, 0);
    }

    for (size_t i = 0; i < neurons_count; ++i)
    {
        for (size_t j = 0; j < i; ++j)
        {
            neuron_type::coefficient val = 0;
            val = std::accumulate(
                      begin(src_images),
                      end(src_images),
                      neuron_type::coefficient(0.0),
                      [i, j] (neuron_type::coefficient old_val, const neurons_line &image) -> neuron_type::coefficient{
                                                                        return old_val + (image[i] * image[j]);
                                                                    });
            result_coeffs[i][j] = val;
            result_coeffs[j][i] = val;
        }
    }

    return result_coeffs;
}

struct neurons_line_print_descriptor
{
    const neurons_line &_line;
    const size_t _width;
    const size_t _height;

    neurons_line_print_descriptor (
        const neurons_line &line,
        size_t width,
        size_t height
    ): _line(line),
        _width(width),
        _height(height)
    {}
};

template <typename Ch, typename Tr>
std::basic_ostream<Ch, Tr>&
operator << (std::basic_ostream<Ch, Tr>&stm, const neurons_line_print_descriptor &line)
{
    neurons_line::const_iterator it = begin(line._line), it_end = end(line._line);

    for (size_t i = 0; i < line._height; ++i)
    {
        for (size_t j = 0; j < line._width; ++j)
        {
            stm << neuron_type::write(*it);
            ++it;
        }
        stm << endl;
    }

    return stm;
}

neurons_line read_neurons_state(const std::string &file_path, size_t len)
{
    std::ifstream i_file(file_path);

    size_t i = 0;
    neurons_line result;
    result.reserve(len);

    while (i++ < len)
    {
        char val;
        i_file >> val;
        auto state = neuron_type::read(val);
        result.push_back(state);
    }

    return result;
}

int main()
{
    list<neurons_line> src_images;
    string target_file_path, object_to_see, read_symbol;
    size_t width, height;

    cin >> width >> height;

    do
    {
        cin >> read_symbol;
        if (read_symbol != ".") {
            target_file_path = read_symbol;
            src_images.push_back(read_neurons_state(target_file_path + ".txt", width * height));

            cout << "Image to learn: " << endl;
            cout << neurons_line_print_descriptor(src_images.back(), width, height) << endl;
        }
    }
    while (read_symbol != ".");

    while (1 == 1) {
        cout << "Enter an object to recognize:" << endl;
        cin >> target_file_path;
        target_file_path += ".txt";
        link_coeffs coeffs = learn_neuro_net(src_images);

        neuro_net_system net(coeffs);

        neurons_line line;
        line = read_neurons_state(target_file_path, width * height);

        cout << "START: " << endl;
        cout << neurons_line_print_descriptor(line, width, height) << endl;

        cout << "END: " << endl;
        cout << neurons_line_print_descriptor(line, width, height) << endl;
    }

    return 0;
}
