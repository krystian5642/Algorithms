# Algorithms

The application is built in Qt for studying time complexity of graph algorithms and visualizing graph algorithms.

## Table of Contents
1. [Visualization Tab](#visualization-tab)
   - [How it works](#how-it-works)
   - [Visualization Panel Actions](#visualization-panel-actions)
   - [Options](#options)
2. [Benchmark Tab](#benchmark-tab)
   - [How it works](#how-it-works-1)
   - [Interpreting the Chart](#interpreting-the-chart)
   - [Benchmark Panel Actions](#benchmark-panel-actions)

---

# Visualization Tab

This tab consists of two main parts: an algorithms panel on the left and a main panel for creating graphs on the right.

## How it works

Firstly, you need to choose an algorithm by clicking on it. After clicking, a new panel shows up below the algorithms list. The panel contains all the chosen algorithm's properties such as step time, start, end, etc. The panel is automatically generated using Qt meta-system.

To start an algorithm, you need a non-empty graph. To create a graph, you have two options:

- Click the dice icon in the top panel  
- Click on the white space to the right of the algorithms; it will add a new node to the graph

## Visualization Panel Actions

- **Save** – saves the graph to a JSON file (serialization).
- **Load** – loads a graph from a JSON file (deserialization).
- **Clean** – clears the current graph.
- **Generate graph** – generates a graph based on selected parameters.
- **Play** – starts step-by-step visualization of the algorithm.
- **Generate random edges** – adds random edges to the graph.
- **Add edge** – adds an edge according to specified values.
- **Remove edge** – removes an edge between nodes.
- **Clear graph color** – resets graph colors to black (color changes during algorithm animation).

## Options

(booleans / checkboxes)

- **Show weights** – enables the display of edge weights.
- **Graph directed** – determines whether the graph is directed or not; cannot be changed after adding the first edge.

---

# Benchmark Tab

This tab is designed for testing the time complexity of algorithms.

## How it works

Firstly, you need to choose an algorithm by clicking on it. After clicking, a new panel shows up below the algorithms list. The panel contains all the chosen algorithm's properties such as number of iterations, complexity aspect being tested, etc.

After choosing an algorithm, to start the benchmark you need to click the play button. When the benchmark finishes, results are displayed in the main area as a new series.

Benchmarks can take a very long time since they repeat the same algorithm many times, but on increasingly larger graphs. There is a possibility to stop the benchmark and get the current result by clicking the pause button (play turns into pause after clicking and starting the benchmark).

The benchmark runs on another thread, so all communication happens through events.

It is possible to perform many tests of the same or different algorithms – new series will be added to the main chart. By clicking on a series, you can see more descriptive information about the performed benchmark. To remove a series, click its name above the main chart.

## Interpreting the Chart

- A line close to a straight line – the selected algorithm complexity is correct.
- A curve resembling a logarithmic function – the selected complexity is too high.
- A curve growing like a parabola – the selected complexity is too low.

## Benchmark Panel Actions

- **Save** – saves the last series to JSON.
- **Load** – loads a series from JSON.
- **Clean** – removes all series from the screen.
- **Play / Pause** – starts the benchmark; clicking again stops the process and displays the current result.
- **Clear spikes** – removes individual disturbances (noise) from the series.
- **Debug run** – (debug version only) runs the algorithm and prints the result in the console.
