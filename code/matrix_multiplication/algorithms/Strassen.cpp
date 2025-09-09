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
const fs::path INPUT_DIR  = "../data";
const fs::path OUTPUT_DIR = "../measurements/matrix_multiplication";

vector<vector<int>> strassen(const vector<vector<int>>& A, const vector<vector<int>>& B) {
    int n = A.size();
    vector<vector<int>> C(n, vector<int>(n, 0));
    if (n == 1) {
        C[0][0] = A[0][0] * B[0][0];
        return C;
    }
    int k = n / 2;
    // Submatrices
    vector<vector<int>> 
        A11(k, vector<int>(k)), A12(k, vector<int>(k)), A21(k, vector<int>(k)), A22(k, vector<int>(k)),
        B11(k, vector<int>(k)), B12(k, vector<int>(k)), B21(k, vector<int>(k)), B22(k, vector<int>(k));
    for (int i = 0; i < k; ++i) {
        for (int j = 0; j < k; ++j) {
            A11[i][j] = A[i][j];
            A12[i][j] = A[i][j + k];
            A21[i][j] = A[i + k][j];
            A22[i][j] = A[i + k][j + k];
            B11[i][j] = B[i][j];
            B12[i][j] = B[i][j + k];
            B21[i][j] = B[i + k][j];
            B22[i][j] = B[i + k][j + k];
        }
    }
    // S matrices
    auto add = [](const vector<vector<int>>& X, const vector<vector<int>>& Y) {
        int n = X.size();
        vector<vector<int>> R(n, vector<int>(n));
        for (int i = 0; i < n; ++i)
            for (int j = 0; j < n; ++j)
                R[i][j] = X[i][j] + Y[i][j];
        return R;
    };
    auto sub = [](const vector<vector<int>>& X, const vector<vector<int>>& Y) {
        int n = X.size();
        vector<vector<int>> R(n, vector<int>(n));
        for (int i = 0; i < n; ++i)
            for (int j = 0; j < n; ++j)
                R[i][j] = X[i][j] - Y[i][j];
        return R;
    };
    // 7 products
    auto M1 = strassen(add(A11, A22), add(B11, B22));
    auto M2 = strassen(add(A21, A22), B11);
    auto M3 = strassen(A11, sub(B12, B22));
    auto M4 = strassen(A22, sub(B21, B11));
    auto M5 = strassen(add(A11, A12), B22);
    auto M6 = strassen(sub(A21, A11), add(B11, B12));
    auto M7 = strassen(sub(A12, A22), add(B21, B22));
    // Combine results
    for (int i = 0; i < k; ++i) {
        for (int j = 0; j < k; ++j) {
            C[i][j] = M1[i][j] + M4[i][j] - M5[i][j] + M7[i][j];
            C[i][j + k] = M3[i][j] + M5[i][j];
            C[i + k][j] = M2[i][j] + M4[i][j];
            C[i + k][j + k] = M1[i][j] - M2[i][j] + M3[i][j] + M6[i][j];
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
    auto C = strassen(A, B);
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

// https://www.tutorialspoint.com/data_structures_algorithms/strassens_matrix_multiplication_algorithm.htm