#!/usr/bin/gawk -f
#
# Usage: generate_table.awk < ${board}.txt
#
# Takes the *.txt file generated by AutoBenchmark.ino and generates an ASCII
# table that can be inserted into the README.md. Collects both sizeof()
# information as well as CPU benchmarks.

BEGIN {
  # Set to 1 when 'SIZEOF' is detected
  collect_sizeof = 0

  # Set to 1 when 'BENCHMARKS' is detected
  collect_benchmarks = 0
}

/^SIZEOF/ {
  collect_sizeof = 1
  collect_benchmarks = 0
  sizeof_index = 0
  next
}

/^BENCHMARKS/ {
  collect_sizeof = 0
  collect_benchmarks = 1
  benchmark_index = 0
  next
}

!/^END/ {
  if (collect_sizeof) {
    s[sizeof_index] = $0
    sizeof_index++
  }
  if (collect_benchmarks) {
    u[benchmark_index]["name"] = $1
    u[benchmark_index]["min"] = $2
    u[benchmark_index]["avg"] = $3
    u[benchmark_index]["max"] = $4
    u[benchmark_index]["samples"] = $5
    benchmark_index++
  }
}

END {
  TOTAL_BENCHMARKS = benchmark_index
  TOTAL_SIZEOF = sizeof_index

  printf("Sizes of Objects:\n")
  for (i = 0; i < TOTAL_SIZEOF; i++) {
    print s[i]
  }

  print ""
  print "CPU:"

  printf("+-----------------------------------------+-------------------+---------+\n")
  printf("| Functionality                           |   min/  avg/  max | eff kHz |\n")
  for (i = 0; i < TOTAL_BENCHMARKS; i++) {
    name = u[i]["name"]
    if (name ~ /^TwoWireInterface<TwoWire>,100kHz/ \
    ) {
      printf("|-----------------------------------------+-------------------+---------|\n")
    }

    # 11 bytes, 9 bits/byte
    printf("| %-39s | %5d/%5d/%5d |   %5.1f |\n",
      u[i]["name"], u[i]["min"], u[i]["avg"], u[i]["max"],
      1000.0 * 11 * 9 / u[i]["avg"])
  }
  printf("+-----------------------------------------+-------------------+---------+\n")
}
