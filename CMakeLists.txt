cmake_minimum_required(VERSION 3.24)

# 1. Tu definiujemy, że projekt używa C (dla Twojego kodu) i CXX (C++ dla GoogleTest)
project(JakisTestC C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)

# ==========================================
# 2. Pobieranie Google Test (Automatycznie)
# ==========================================
include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/heads/main.zip
)
# To wycisza ostrzeżenia z biblioteki Google (opcjonalne, ale przydatne)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# ==========================================
# 3. Twój Główny Program (Aplikacja)
# ==========================================
# To jest to, co uruchamiasz, żeby bawić się blockchainem
# WAŻNE: Tutaj NIE DODAJESZ plików z folderu tests/
add_executable(JakisTestC
        src/main.c
        src/blockchain_helpers.c
        src/blockchain_helpers.h
        src/sha256.c
        src/sha256.h
)

# ==========================================
# 4. Twój Program Testowy (Unit Testy)
# ==========================================
# To jest osobny plik .exe, który tylko sprawdza czy kod działa
# WAŻNE: Tutaj NIE DODAJESZ main.c (bo testy mają swój własny main)
add_executable(BlockchainTests
        tests/tests_main.cpp      # Plik z testami (C++)
        src/blockchain_helpers.c            # Twoja logika (C)
        src/blockchain_helpers.h            # Nagłówki (C)
)

# Linkujemy bibliotekę Google Test do Twoich testów
target_link_libraries(BlockchainTests PRIVATE gtest_main)