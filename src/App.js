// App.jsx
import React from 'react';
import './App.css';

// Sample data structure (replace with your actual data)
const projectData = {
  datasets: [
    {
      name: "Dataset 1",
      largestClique: 15,
      totalCliques: 245,
      executionTime: 1.23, // seconds
      cliqueDistribution: [0, 0, 5, 10, 25, 40, 60, 50, 30, 15, 8, 2, 0, 0, 1]
    },
    {
      name: "Dataset 2",
      largestClique: 12,
      totalCliques: 187,
      executionTime: 0.89,
      cliqueDistribution: [0, 0, 8, 15, 30, 45, 35, 25, 15, 8, 4, 2]
    }
  ],
  algorithms: [
    {
      name: "CLIQUE (Worst-case)",
      executionTimes: [1.23, 0.89] // seconds per dataset
    },
    {
      name: "BronkerboschDegeneracy",
      executionTimes: [0.95, 0.72]
    },
    {
      name: "CLIQUE (Arboricity)",
      executionTimes: [1.05, 0.78]
    }
  ]
};

function App() {
  return (
    <div className="App">
      <header>
        <h1>Maximal Clique Enumeration Project</h1>
        <p>Implementation and Analysis of Three Algorithms</p>
      </header>

      <section className="project-info">
        <h2>Project Overview</h2>
        <p>
          This project implements and compares three maximal clique enumeration algorithms:
          CLIQUE (Worst-case), BronkerboschDegeneracy, and CLIQUE (Arboricity).
          The implementations are written in C/C++ and tested on multiple graph datasets.
        </p>
      </section>

      <section className="results">
        <h2>Experimental Results</h2>
        
        <div className="dataset-stats">
          <h3>Dataset Statistics</h3>
          {projectData.datasets.map((dataset, index) => (
            <div key={index} className="dataset-card">
              <h4>{dataset.name}</h4>
              <p>Largest Clique Size: {dataset.largestClique}</p>
              <p>Total Maximal Cliques: {dataset.totalCliques}</p>
              <div className="clique-distribution">
                <h5>Clique Size Distribution</h5>
                <div className="histogram">
                  {dataset.cliqueDistribution.map((count, size) => (
                    count > 0 && (
                      <div 
                        key={size}
                        className="bar"
                        style={{ height: `${count * 5}px` }}
                        title={`Size ${size}: ${count} cliques`}
                      >
                        <span>{size}</span>
                      </div>
                    )
                  ))}
                </div>
              </div>
            </div>
          ))}
        </div>

        <div className="performance">
          <h3>Performance Comparison</h3>
          <div className="execution-times">
            {projectData.datasets.map((dataset, dIndex) => (
              <div key={dIndex} className="dataset-performance">
                <h4>{dataset.name}</h4>
                <div className="time-histogram">
                  {projectData.algorithms.map((algo, aIndex) => (
                    <div 
                      key={aIndex}
                      className="time-bar"
                      style={{ height: `${algo.executionTimes[dIndex] * 100}px` }}
                    >
                      <span>{algo.name}: {algo.executionTimes[dIndex]}s</span>
                    </div>
                  ))}
                </div>
              </div>
            ))}
          </div>
        </div>
      </section>

      <section className="observations">
        <h2>Observations</h2>
        <ul>
          <li>BronkerboschDegeneracy consistently showed the best performance across datasets</li>
          <li>Larger clique sizes correlate with increased execution time</li>
          <li>Sparse graphs benefited more from the Arboricity-based algorithm</li>
          <li>The worst-case CLIQUE algorithm performed better on dense graphs</li>
        </ul>
      </section>

      <footer>
        <p>Created by [Your Name] | March 23, 2025</p>
        <p>Source code available on <a href="#">GitHub</a></p>
      </footer>
    </div>
  );
}

export default App;