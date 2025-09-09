#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <filesystem>
#include <algorithm>
#include <chrono>
using namespace std;
namespace fs = std::filesystem;

// === Configuración ===
const fs::path INPUT_DIR  = "code/matrix_multiplication/data/matrix_input";
const fs::path OUTPUT_DIR = "code/matrix_multiplication/data/matrix_output";

// Function to perform naive matrix multiplication
std::vector<std::vector<int>> naiveMatrixMultiplication(
    const std::vector<std::vector<int>>& A,
    const std::vector<std::vector<int>>& B) {

    // Get dimensions of matrices
    int rowsA = A.size();
    int colsA = A[0].size();
    int rowsB = B.size();
    int colsB = B[0].size();

    // Check for compatible dimensions
    if (colsA != rowsB) {
        std::cerr << "Error: Incompatible matrix dimensions for multiplication." << std::endl;
        return {}; // Return an empty matrix or handle the error appropriately
    }

    // Initialize the result matrix C with appropriate dimensions
    std::vector<std::vector<int>> C(rowsA, std::vector<int>(colsB, 0));

    // Perform the multiplication
    for (int i = 0; i < rowsA; ++i) { // Iterate through rows of A
        for (int j = 0; j < colsB; ++j) { // Iterate through columns of B
            for (int k = 0; k < colsA; ++k) { // Iterate through common dimension
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
    return C;
}

// ---- Leer una matriz desde archivo ----
vector<vector<int>> leerMatriz(const fs::path& file, int& n) {
    ifstream in(file);
    in >> n; // se asume que el primer número del archivo es la dimensión
    vector<vector<int>> M(n, vector<int>(n));
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            in >> M[i][j];
    in.close();
    return M;
}

// ---- Guardar matriz en archivo ----
void guardarMatriz(const fs::path& file, const vector<vector<int>>& M) {
    ofstream out(file);
    int n = M.size();
    out << n << "\n";
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++)
            out << M[i][j] << " ";
        out << "\n";
    }
    out.close();
}

// ---- Procesar un par de archivos ----
void procesarCaso(const fs::path& file1, const fs::path& file2, const fs::path& outDir) {
    int n1, n2;
    auto A = leerMatriz(file1, n1);
    auto B = leerMatriz(file2, n2);

    size_t memBytes = (A.size() * A.size() + B.size() * B.size()) * sizeof(int);
    cout << "Procesando " << file1.filename().string()
         << " + " << file2.filename().string()
         << " | n=" << n1
         << " | ~mem=" << (memBytes / (1024.0*1024.0)) << " MB" << endl;

    auto start = chrono::high_resolution_clock::now();
    auto C = naiveMatrixMultiplication(A, B);
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end - start;

    // Construir nombre de salida a partir de file1
    string baseName = file1.stem().string();
    baseName = baseName.substr(0, baseName.size() - 2); // quitar "_1"
    fs::path outFile = outDir / (baseName + "_out.txt");

    guardarMatriz(outFile, C);

    cout << "✅ " << outFile.string() << " | tiempo=" << elapsed.count() << " s\n";
}

// ---- Main ----
int main() {
    fs::create_directories(OUTPUT_DIR);

    // Recolecta archivos *_1.txt
    vector<fs::directory_entry> files;
    for (const auto& entry : fs::directory_iterator(INPUT_DIR)) {
        string name = entry.path().filename().string();
        if (entry.is_regular_file() && entry.path().extension() == ".txt" &&
            name.find("_1.txt") != string::npos) {
            files.push_back(entry);
        }
    }

    // Ordenar por tamaño y fecha
    sort(files.begin(), files.end(),
        [](const fs::directory_entry& a, const fs::directory_entry& b) {
            auto sa = fs::file_size(a.path());
            auto sb = fs::file_size(b.path());
            if (sa == sb) return fs::last_write_time(a) < fs::last_write_time(b);
            return sa < sb;
        });

    auto globalStart = chrono::high_resolution_clock::now();
    for (const auto& e : files) {
        string base = e.path().stem().string();   // ej: "28_densa_D10_b_1"
        string other = base.substr(0, base.size() - 1) + "2"; // reemplazar "_1" por "_2"
        fs::path file2 = e.path().parent_path() / (other + ".txt");

        if (fs::exists(file2)) {
            procesarCaso(e.path(), file2, OUTPUT_DIR);
        } 
    }
    auto globalEnd = chrono::high_resolution_clock::now();

    cout << "\nTiempo total: "
         << chrono::duration<double>(globalEnd - globalStart).count()
         << " s\n";
    return 0;
}