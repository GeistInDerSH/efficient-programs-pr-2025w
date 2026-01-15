#!/bin/bash

binary=$1
board=$2
runs=5
measure_overhead=0
while test $# -gt 0; do
  case "$1" in
    -h|--help)
      echo "$0 - Measure the average runtime of a binary evaluating a Sudoku board"
      echo " "
      echo "$0 --binary sudoku_binary --board sudoku_board [--runs int] [--measure-shell-overhead]"
      echo "$0 sudoku_binary sudoku_board"
      echo " "
      echo "options:"
      echo "--binary                  The path to the sudoku binary to run."
      echo "                          If the program needs an interpreter, e.g. Python/Java,"
      echo '                          put it in quotes, e.g. --binary "java -jar target.jar"'
      echo "--board                   The path to the sudoku board to pass to the program."
      echo "--runs                    The number of runs to do, for the average. Default ${runs}"
      echo "                          Cannot be set to a value less than 3"
      echo "--measure-shell-overhead  Before running the measurements, try to determine the"
      echo "                          overhead of timing via the shell."
      exit 0
      ;;
    --binary)
      shift
      if test $# -gt 0; then
        export binary=$1
      else
        echo "no binary specified"
        exit 1
      fi
      shift
      ;;
    --board*)
      shift
      if test $# -gt 0; then
        export board=$1
      else
        echo "no sudoku board specified"
        exit 1
      fi
      shift
      ;;
    --runs)
      shift
      if test $# -gt 0; then
        if [[ $1 -lt 3 ]]; then
          echo "[WARN] Too few runs given ($1), setting to 3 to ensure at least one run is run"
          export runs=3
        else
          export runs=$1
        fi
      else
        echo "no runs specified"
        exit 1
      fi
      shift
      ;;
    --measure-shell-overhead)
      export measure_overhead=1
      shift
      ;;
    *)
      shift
      ;;
  esac
done

function measure_average_runtime () {
  local binary=$1
  local board=$2
  local runs=$3

  # Run the program with the input, and append the runtime in ns to a list
  times=()
  for _ in $(seq "${runs}"); do
    time_start=$(date +%s%N)
    $binary $board > /dev/null
    time_end=$(date +%s%N)
    runtime_ns=$((time_end-time_start))
    times+=("$runtime_ns")
  done

  # Steps:
  # 1) Sort the times by their numeric value
  # 2) Split them into lines
  # 3) Remove the longest/slowest time (as it's likely the warmup run)
  # 4) Remove the fastest run
  without_fastest_and_slowest=$(echo "${times[@]}" | sort -n | tr ' ' '\n' | tail -n $((runs-1)) | head -n $((runs-2)))

  total_time_ns=0
  counted_runs=$((runs-2))
  for t in ${without_fastest_and_slowest}; do
    total_time_ns=$((t+total_time_ns))
  done

  average_time_ns=$((total_time_ns/counted_runs))
  echo "$average_time_ns"
}

# Try to account for the overhead of running cmds in the shell.
# Namely `date` because the measurement will include the runtime
# of our program, and the ending call to `date`.
average_startup_time_ns=0
if [ 1 -eq "$measure_overhead" ]; then
  average_startup_time_ns=$(measure_average_runtime "date" "+%s%N" "$runs")
fi

average_time_ns=$(measure_average_runtime "$binary" "$board" "$runs")
difference=$((average_time_ns-average_startup_time_ns))
if [ $difference -lt 0 ]; then
  echo "[WARN] Measured shell overhead is larger than program. Ignoring overhead"
  average_startup_time_ns=0
fi

average_time_ns=$((average_time_ns-average_startup_time_ns))
average_time_us=$((average_time_ns/1000))
average_time_ms=$((average_time_us/1000))

echo "Program: '${binary} ${board}'"
echo "Wall Clock Time:"
echo "Time (ns): ${average_time_ns}"
echo "Time (μs): ${average_time_us}"
echo "Time (ms): ${average_time_ms}"