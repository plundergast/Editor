#pragma once

#include <algorithm>
#include <string>
#include <vector>
#include <iostream>

template<typename T>
constexpr T clamp(T value, T min, T max) {
    if(value > max)
	return max;
    else if(value < min)
	return min;
    else
	return value;
}

struct buffer {
    buffer(unsigned capacity = 4096) : capacity(capacity) { buff = new char[capacity]; }
    buffer(const buffer& other) : capacity(other.capacity),
				  size(other.size),
				  cursor_pos(other.cursor_pos) {
	buff = new char[capacity];
	for(int i = 0; i < size; i++)
	    buff[i] = other.buff[i];
    }
    buffer(buffer&& other) : capacity(std::move(other.capacity)),
			     size(std::move(other.size)),
			     cursor_pos(std::move(other.cursor_pos)) {
	buff = std::move(other.buff);
	other.buff = nullptr;
    }
    ~buffer() { delete[] buff; }

    buffer& operator=(const buffer& other) {
	capacity = other.capacity;
	size = other.size;
	cursor_pos = other.cursor_pos;

	buff = new char[capacity];
	for(int i = 0; i < size; i++)
	    buff[i] = other.buff[i];

	return *this;
    }
    buffer& operator=(buffer&& other) {
	capacity = std::move(other.capacity);
	size = std::move(other.size);
	cursor_pos = std::move(other.cursor_pos);
	buff = std::move(other.buff);
	other.buff = nullptr;
	return *this;
    }

    buffer(const std::vector<std::string>& v) : capacity(4096) {
	buff = new char[capacity];
	reset();
	add_lines_vec(v);
	cursor_pos = 0;
    }
    buffer& operator=(const std::vector<std::string>& v){
	reset();
	add_lines_vec(v);
	return *this;
    }

    /// Storage.
    char* buff; // 4kb of storage, should be enough.

    /// Size of allocated character buffer. Usually useless, use size instead.
    unsigned capacity;

    /// Size of currently used part of buffer.
    unsigned size = 0;

    /// Position of cursor in buffer.
    int cursor_pos = 0;

    void left(unsigned amt = 1) {
	// if(buff[cursor_pos - 1] == '\n') // Skip any new lines
	//     cursor_pos--;
	cursor_pos = clamp<int>(cursor_pos - amt, 0, size);
    }

    void right(unsigned amt = 1) {
	// if(buff[cursor_pos + 1] == '\n') // Skip any new lines
	//     cursor_pos++;
	cursor_pos = clamp<int>(cursor_pos + amt, 0, size);
    }

    /// Moves the cursor to the beginning of the previous line.
    void prev_line() {
	seek_bol();
	left(1);
	seek_bol();
    }

    /// Moves the cursor to the beginning of the next line.
    void next_line() {
	seek_eol();
	right(1);
    }

    void down() {
	int x_offset = get_cursor_x();
	next_line();

	if(curr_line_length() < x_offset) {
	    seek_eol();
	} else {
	    right(x_offset);
	}
    }

    void up() {
	int x_offset = get_cursor_x();
	prev_line();

	if(curr_line_length() < x_offset)
	    seek_eol();
	else
	    right(x_offset);
    }

    void reset() { size = 0; cursor_pos = 0; }

    void add_lines_vec(const std::vector<std::string>& vec) {
	for(const std::string& s : vec)
	{
	    set_current_line(s);
	    insert_at_cursor('\n');
	}
    }

    inline operator std::vector<std::string>() const { return as_lines_vec(); }
    std::vector<std::string> as_lines_vec() const {
	int orig_pos = cursor_pos;

	std::vector<std::string> res;
	res.reserve(num_lines());

	((buffer*)(this))->cursor_pos = 0;

	for(int i = 0; i < num_lines(); i++) {
	    res.push_back(get_current_line());
	    ((buffer*)(this))->next_line();
	}

	((buffer*)(this))->cursor_pos = orig_pos;

	return res;
    }

    inline operator std::string() const { return as_string(); }
    inline std::string as_string() const { return std::string(buff, size); }
    
    int get_cursor_x() const { return cursor_pos - find_bol(); }
    int get_cursor_y() const { return curr_line_number(); }

    unsigned curr_line_number() const { return std::count(buff, buff + cursor_pos, '\n'); }
    unsigned curr_line_length() const { int e = find_eol(); int b = find_bol(); return e-b;}
    std::string get_current_line() const {
	int b = find_bol();
	return std::string(buff + b, curr_line_length());
    }
    void set_current_line(std::string line) {
	seek_bol();
	while(size > 0
	      && char_under_cursor() != '\n'
	      && cursor_pos != find_eol())
	    delete_after_cursor();

	for(int i = 0; i < line.length() && line[i] != '\n'; i++)
	{
	    insert_at_cursor(line[i]);
	}
    }
    void delete_line() {
	seek_bol();
	while(size > 0 && char_under_cursor() != '\n' && cursor_pos != find_eol())
	    delete_after_cursor();
	delete_after_cursor();
    }


    inline unsigned num_lines() const { return std::max<unsigned>(1, std::count(buff, buff + size, '\n')); }

    void insert_at_cursor(char c) {
	if(size != 0)
	    for(int i = size + 1; i > cursor_pos; i--)
		buff[i] = buff[i-1];

	size = clamp<unsigned>(size+1, 0, capacity);

	buff[cursor_pos] = c;
	right();
    }

    /// Returns the character the cursor is marking.
    char char_under_cursor() { return buff[cursor_pos]; }

    /// Finds the index for the beginning of the current line.
    int find_bol() const {
	for(int i = cursor_pos; i > 0; i--) {
	    if(buff[i-1] == '\n') { return i; }
	}
	return 0;
    }

    /// Finds the index for the end of the current line.
    int find_eol() const { 
	for(int i = cursor_pos; i <= size; i++) {
	    if(buff[i] == '\n') return i;
	}
	return size;
    }

    /// Moves the cursor to the beginning of the current line
    /// or to the end of the buffer.
    void seek_bol() { cursor_pos = find_bol(); }

    /// Moves the cursor to the end of the current line
    /// or to the end of the buffer.
    void seek_eol() { cursor_pos = find_eol(); }

    /// Deletes the character that the cursor is on and moves it backwards.
    /// Equivalent to pressing the Backspace key.
    void delete_at_cursor() {
	if(size < 1 || cursor_pos == 0)
	    return;

	size--;
	left();

	for(int i = cursor_pos; i < size; i++)
	    buff[i] = buff[i+1];

	// buff[cursor_pos] = 0;
    }

    /// Deletes the character that is after the cursor
    /// without moving the cursor itself.
    /// Equivalent to pressing the Delete key.
    void delete_after_cursor() {
	if(size < 1 || cursor_pos == size)
	    return;

	size--;

	for(int i = cursor_pos; i < size; i++)
	    buff[i] = buff[i+1];

	// buff[cursor_pos] = 0;
    }
};
