#pragma once
#include "../include/Sequences/ArraySequence.h"
#include "../include/Sequences/ListSequence.h"
#include "../include/Sequences/SequenceFunctions.h"
#include "../include/Specialized/BitSequence.h"
#include "../include/Base/DynamicArray.h"
#include "../include/Common/Option.h"
#include <iostream>
#include <string>
#include <functional>
#include <chrono>
#include <limits>

template<typename T>
void update_ptr(T*& ptr, T* newPtr) {
    if (ptr == newPtr) return;
    delete ptr;
    ptr = newPtr;
}

double benchmark(std::function<void()> func) {
    auto start = std::chrono::high_resolution_clock::now();
    func();
    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double, std::milli>(end - start).count();
}

Sequence<int>* create_seq(int type, int* data, int count) {
    if (type == 1) return new MutableArraySequence<int>(data, count);
    if (type == 2) return new ImmutableArraySequence<int>(data, count);
    if (type == 3) return new MutableListSequence<int>(data, count);
    if (type == 4) return new ImmutableListSequence<int>(data, count);
    return nullptr;
}

// Вспомогательная функция для ввода целочисленной последовательности (возвращает сырой массив, длину по ссылке)
int* input_int_sequence(const std::string& prompt, int& out_length) {
    std::cout << prompt << " (count): ";
    int n = 0;
    if (!(std::cin >> n) || n < 0) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        out_length = 0;
        return nullptr;
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    if (n == 0) {
        out_length = 0;
        return new int[0];
    }
    int* buf = new int[n];
    std::cout << "Enter " << n << " elements: ";
    for (int i = 0; i < n; ++i) {
        if (!(std::cin >> buf[i])) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            delete[] buf;
            out_length = 0;
            return nullptr;
        }
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    out_length = n;
    return buf;
}

void run_interface() {
    Sequence<int>* seq = nullptr;
    BitSequence* bit_seq = nullptr;
    int type = 0;
    
    std::cout << "Select sequence type: 1-MutableArray, 2-ImmutableArray, 3-MutableList, 4-ImmutableList, 5-Bit\n>> ";
    if (!(std::cin >> type)) {
        std::cin.clear(); std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cerr << "Invalid input. Exiting.\n";
        return;
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    try {
        if (type == 5) {
            bit_seq = new BitSequence();
            std::string input; 
            std::cout << "Enter bit string (0/1): "; 
            std::cin >> input;
            for (char c : input) bit_seq->Append(c == '1');
        } else {
            int n = 0;
            std::cout << "Enter count: ";
            if (!(std::cin >> n) || n < 0) {
                std::cin.clear(); std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cerr << "Invalid count.\n"; return;
            }
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            int* buffer = new int[n];
            std::cout << "Enter " << n << " elements: ";
            for (int i = 0; i < n; ++i) {
                if (!(std::cin >> buffer[i])) {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    std::cerr << "\nInvalid number detected. Skipping remaining inputs.\n";
                    delete[] buffer;
                    return;
                }
            }
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            seq = create_seq(type, buffer, n);
            delete[] buffer;
            std::cout << "Sequence created (" << seq->GetLength() << " items)." << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Init error: " << e.what() << std::endl;
        return;
    }

    int choice = -1;
    while (true) {
        std::cout << "\n--- MENU ---\n"
                  << "1.Append  2.Prepend  3.InsertAt  4.GetSubsequence  5.Concat\n"
                  << "6.GetEnumerator  7.Map  8.Where  9.Reduce  10.Zip  11.Stats  12.BitOps\n"
                  << "13.Split  14.Slice  15.Unzip  16.FlatMap  17.Skip  18.TryFind  0.Exit\n>> ";
        
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input. Please enter a number (0-18).\n";
            continue;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (choice < 0 || choice > 18) {
            std::cout << "Invalid choice. Please enter a number between 0 and 18.\n";
            continue;
        }
        if (choice == 0) break;

        try {
            if (type == 5) {
                // ... (без изменений, BitOps уже есть)
                // для краткости оставлю старую логику, но новые пункты для BitSequence не добавляю
                if (choice == 12) {
                    BitSequence* other = new BitSequence();
                    std::string input; std::cout << "Enter second bit string: "; std::cin >> input;
                    for (char c : input) other->Append(c == '1');
                    std::cout << "AND: "; bit_seq->Print(*other, [](bool a, bool b){ return a & b; });
                    std::cout << "\nOR: "; bit_seq->Print(*other, [](bool a, bool b){ return a | b; });
                    std::cout << "\nXOR: "; bit_seq->Print(*other, [](bool a, bool b){ return a ^ b; });
                    std::cout << "\nNOT: "; bit_seq->Print(*bit_seq, [](bool a, bool){ return !a; });
                    std::cout << std::endl; delete other;
                } else if (choice >= 1 && choice <= 18) {
                    std::cout << "This operation is not supported for BitSequence in this UI.\n";
                }
                continue;
            }

            // --- Обработка новых команд ---
            if (choice == 13) { // Split по заданному значению
                int delimiter;
                std::cout << "Enter delimiter value: ";
                std::cin >> delimiter;
                auto splitRes = SequenceFunctions::Split(seq, [delimiter](int x) { return x == delimiter; });
                std::cout << "Split into " << splitRes->GetLength() << " parts:\n";
                for (int i = 0; i < splitRes->GetLength(); ++i) {
                    Sequence<int>* part = splitRes->Get(i);
                    std::cout << "[";
                    for (int j = 0; j < part->GetLength(); ++j)
                        std::cout << part->Get(j) << (j+1 < part->GetLength() ? ", " : "");
                    std::cout << "]\n";
                    delete part;
                }
                delete splitRes;
            }
            else if (choice == 14) { // Slice
                int start, count;
                std::cout << "Enter start index and count: ";
                std::cin >> start >> count;
                int rlen = 0;
                int* repData = input_int_sequence("Replacement sequence", rlen);
                Sequence<int>* replacement = (repData && rlen > 0) ? new MutableArraySequence<int>(repData, rlen) : nullptr;
                if (repData) delete[] repData;
                auto sliced = SequenceFunctions::Slice(seq, start, count, replacement);
                std::cout << "Result (" << sliced->GetLength() << "): ";
                for (int i = 0; i < sliced->GetLength(); ++i) std::cout << sliced->Get(i) << " ";
                std::cout << std::endl;
                delete sliced;
                delete replacement;
            }
            else if (choice == 15) { // Unzip
                // Для Unzip нужно получить Sequence<Pair<int,int>>. Можно либо сконструировать через Zip из двух seq и потом Unzip.
                // Но проще: предложить пользователю ввести последовательность пар (два массива) и выполнить Unzip над Zip-ом.
                int len1 = 0, len2 = 0;
                int* arr1 = input_int_sequence("First sequence for pairing", len1);
                int* arr2 = input_int_sequence("Second sequence for pairing", len2);
                if (!arr1 || !arr2 || len1 == 0 || len2 == 0) {
                    std::cout << "Invalid input.\n";
                    delete[] arr1; delete[] arr2;
                } else {
                    Sequence<int>* s1 = new MutableArraySequence<int>(arr1, len1);
                    Sequence<int>* s2 = new MutableArraySequence<int>(arr2, len2);
                    auto zipped = SequenceFunctions::Zip(s1, s2);
                    auto unzipped = SequenceFunctions::Unzip(zipped);
                    std::cout << "Unzipped first: ";
                    for (int i = 0; i < unzipped.first->GetLength(); ++i) std::cout << unzipped.first->Get(i) << " ";
                    std::cout << "\nUnzipped second: ";
                    for (int i = 0; i < unzipped.second->GetLength(); ++i) std::cout << unzipped.second->Get(i) << " ";
                    std::cout << std::endl;
                    delete unzipped.first; delete unzipped.second;
                    delete zipped; delete s1; delete s2;
                }
                delete[] arr1; delete[] arr2;
            }
            else if (choice == 16) { // FlatMap
                // Покажем FlatMap с функцией, которая возвращает [x, x*10]
                auto flat = SequenceFunctions::FlatMap(seq, [](int x) {
                    auto inner = new MutableArraySequence<int>();
                    inner->Append(x);
                    inner->Append(x * 10);
                    return inner;
                });
                std::cout << "FlatMap result: ";
                for (int i = 0; i < flat->GetLength(); ++i) std::cout << flat->Get(i) << " ";
                std::cout << std::endl;
                delete flat;
            }
            else if (choice == 17) { // Skip
                int cnt;
                std::cout << "Number of elements to skip: ";
                std::cin >> cnt;
                auto skipped = SequenceFunctions::Skip(seq, cnt);
                std::cout << "After skip: ";
                for (int i = 0; i < skipped->GetLength(); ++i) std::cout << skipped->Get(i) << " ";
                std::cout << std::endl;
                delete skipped;
            }
            else if (choice == 18) { // TryFind
                int val;
                std::cout << "Value to find: ";
                std::cin >> val;
                auto opt = SequenceFunctions::TryFind(seq, [val](int x) { return x == val; });
                if (opt.IsSome()) std::cout << "Found: " << opt.GetValue() << std::endl;
                else std::cout << "Not found.\n";
            }
            // --- Остальные старые команды (1-12) ---
            else if (choice == 1) { int val; std::cout << "Value: "; std::cin >> val; double t = benchmark([&] { update_ptr(seq, seq->Append(val)); }); std::cout << "Time: " << t << " ms\n"; }
            else if (choice == 2) { int val; std::cout << "Value: "; std::cin >> val; double t = benchmark([&] { update_ptr(seq, seq->Prepend(val)); }); std::cout << "Time: " << t << " ms\n"; }
            else if (choice == 3) { int val, idx; std::cout << "Value Index: "; std::cin >> val >> idx; double t = benchmark([&] { update_ptr(seq, seq->InsertAt(val, idx)); }); std::cout << "Time: " << t << " ms\n"; }
            else if (choice == 4) { 
                int s, e; std::cout << "Start End: "; std::cin >> s >> e; 
                Sequence<int>* sub = nullptr;
                double t = benchmark([&] { sub = seq->GetSubsequence(s, e); }); std::cout << "Time: " << t << " ms\n";
                std::cout << "Subsequence: ";
                for(int i=0; i<sub->GetLength(); ++i) std::cout << sub->Get(i) << " "; 
                std::cout << std::endl; 
                delete sub;
            }
            else if (choice == 5) {
                int m, v2;
                std::cout << "Enter second seq count: "; std::cin >> m;
                int* buf2 = new int[m];
                for(int i=0; i<m; ++i) { std::cin >> v2; buf2[i] = v2; }
                auto s2 = create_seq(type, buf2, m);
                delete[] buf2;
                double t = benchmark([&] { update_ptr(seq, seq->Concat(s2)); }); std::cout << "Time: " << t << " ms\n"; 
                delete s2;
            }
            else if (choice == 6) {
                auto it = seq->GetEnumerator();
                std::cout << "Elements: ";
                while(it->MoveNext()) std::cout << it->GetCurrent() << " "; 
                std::cout << std::endl; 
                delete it;
            }
            else if (choice == 7) {
                update_ptr(seq, SequenceFunctions::Map(seq, [](int x) { return x * 2; }));
                std::cout << "Map completed (x * 2). New length: " << seq->GetLength() << std::endl;
            }
            else if (choice == 8) {
                update_ptr(seq, SequenceFunctions::Where(seq, [](int x) { return x > 0; }));
                std::cout << "Filter completed (x > 0). New length: " << seq->GetLength() << std::endl;
            }
            else if (choice == 9) { int sum = SequenceFunctions::Reduce(seq, 0, [](int a, int b) { return a + b; }); std::cout << "Sum: " << sum << std::endl; }
            else if (choice == 10) {
                int m, v2;
                std::cout << "Enter second seq count: "; std::cin >> m;
                int* buf2 = new int[m];
                for(int i=0; i<m; ++i) { std::cin >> v2; buf2[i] = v2; }
                auto s2 = create_seq(type, buf2, m);
                delete[] buf2;
                auto zipped = SequenceFunctions::Zip(seq, s2);
                std::cout << "Zipped (" << zipped->GetLength() << " pairs): ";
                for(int i=0; i<zipped->GetLength(); ++i) std::cout << "(" << zipped->Get(i).first << "," << zipped->Get(i).second << ") ";
                std::cout << std::endl; 
                delete zipped; delete s2;
            }
            else if (choice == 11) {
                auto stats = SequenceFunctions::GetMinMaxAvg(seq);
                std::cout << "Min: " << stats.min << " Max: " << stats.max << " Avg: " << stats.avg << std::endl;
                std::cout << "Median: " << SequenceFunctions::GetMedian(seq) << std::endl;
                std::cout << "Inversions: " << SequenceFunctions::CountInversions(seq) << std::endl;
                auto prefs = SequenceFunctions::GetPrefixes(seq); std::cout << "Prefixes count: " << prefs->GetLength() << std::endl; delete prefs;
                auto ma = SequenceFunctions::GetMovingAverage(seq, 2);
                std::cout << "MovingAvg: ";
                for(int i=0; i<ma->GetLength(); ++i) std::cout << ma->Get(i) << " "; 
                std::cout << std::endl; 
                delete ma;
            }
            else if (choice == 12) {
                std::cout << "BitOps only for BitSequence. Restart with type 5.\n";
            }

        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }
    delete seq; 
    delete bit_seq;
    std::cout << "Session ended." << std::endl;
}