#include <algorithm>
#include <vector>
#include <queue>
#include <cmath>
#include <climits>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <filesystem>
#include <algorithm>
#include <chrono>

using namespace std;
namespace fs = std::filesystem;

// === Configura aquí el directorio de entrada y salida ===
const fs::path INPUT_DIR  = "code/sorting/data/array_input";
const fs::path OUTPUT_DIR = "code/sorting/data/array_output";

/* Function to sort array using insertion sort */
void insertionSort(int arr[], int n)
{
    for (int i = 1; i < n; ++i) {
        int key = arr[i];
        int j = i - 1;

        /* Move elements of arr[0..i-1], that are
           greater than key, to one position ahead
           of their current position */
        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            j = j - 1;
        }
        arr[j + 1] = key;
    }
}

// ---- Procesar un archivo con QuickSort y guardar en OUTPUT_DIR ----
void procesarArchivo(const fs::path& inputPath, const fs::path& outDir) {
    ifstream in(inputPath);

    vector<int> arr;
    int x;
    while (in >> x) arr.push_back(x);
    in.close();

    if (arr.empty()) { cerr << "⚠ Archivo vacío: " << inputPath << "\n"; return; }

    size_t memBytes = arr.size() * sizeof(int);
    cout << "Procesando " << inputPath.filename().string()
         << " | n=" << arr.size()
         << " | ~mem=" << (memBytes / (1024.0*1024.0)) << " MB\n";

    auto start = chrono::high_resolution_clock::now();
    insertionSort(arr.data(), arr.size());
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end - start;

    // Construir nombre de salida: <nombre_entrada_sin_ext>_out.txt en otro directorio
    fs::path outName = inputPath.stem().string() + "_out" + inputPath.extension().string();
    fs::path outPath = outDir / outName;

    ofstream out(outPath);
    for (int v : arr) out << v << ' ';
    out << '\n';
    out.close();

    cout << "✅ " << outPath.string() << " | tiempo=" << elapsed.count() << " s\n";
}

int main() {
    // Asegura que el directorio de salida exista
    fs::create_directories(OUTPUT_DIR);

    // Recolecta archivos .txt de INPUT_DIR
    vector<fs::directory_entry> files;
    for (const auto& entry : fs::directory_iterator(INPUT_DIR)) {
        if (entry.is_regular_file() && entry.path().extension() == ".txt") {
            files.push_back(entry);
        }
    }

    // Ordena: primero por tamaño (menor a mayor), luego por fecha (más viejo a más nuevo)
    sort(files.begin(), files.end(),
        [](const fs::directory_entry& a, const fs::directory_entry& b) {
            auto sa = fs::file_size(a.path());
            auto sb = fs::file_size(b.path());
            if (sa == sb) return fs::last_write_time(a) < fs::last_write_time(b);
            return sa < sb;
        });

    auto globalStart = chrono::high_resolution_clock::now();
    for (const auto& e : files) procesarArchivo(e.path(), OUTPUT_DIR);
    auto globalEnd = chrono::high_resolution_clock::now();
    cout << "\nTiempo total: "
         << chrono::duration<double>(globalEnd - globalStart).count()
         << " s\n";

    return 0;
}

/* This code is contributed by Hritik Shah. */
// https://www.geeksforgeeks.org/dsa/insertion-sort-algorithm/