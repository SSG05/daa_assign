import React from 'react';
import './App.css';
import emailEnronImg from './images/email_size.jpg';
import asSkitterImg from './images/ask_size.jpg';
import wikiVoteImg from './images/wiki_vote_size.jpg';

// Project data constants
const PROJECT_DATA = {
  datasets: [
    {
      name: 'email-Enron',
      largestClique: 20,
      totalCliques: 226859,
      cliqueDistributionImage: emailEnronImg,
    },
    {
      name: 'as-Skitter',
      largestClique: 67,
      totalCliques: 37322355,
      cliqueDistributionImage: asSkitterImg,
    },
    {
      name: 'wiki-Vote',
      largestClique: 17,
      totalCliques: 459002,
      cliqueDistributionImage: wikiVoteImg,
    },
  ],
  algorithms: [
    { name: 'Chiba', executionTimes: [86.3, 10807.4 , 44.6] },
    { name: 'ELS', executionTimes: [2.3, 768, 2.8] },
    { name: 'Tomita', executionTimes: [3, 5209.2 , 3.22] },
  ],
};

// Component for displaying dataset information
const DatasetCard = ({ dataset }) => (
  <article className="dataset-card fade-in">
    <h4>{dataset.name}</h4>
    <p>
      <strong>Largest Clique Size:</strong> {dataset.largestClique}
    </p>
    <p>
      <strong>Total Maximal Cliques:</strong> {dataset.totalCliques.toLocaleString()}
    </p>
    <div className="clique-distribution">
      <h5>Clique Size Distribution</h5>
      <img
        src={dataset.cliqueDistributionImage}
        alt={`Clique distribution for ${dataset.name}`}
        width="300"
        height="auto"
      />
    </div>
  </article>
);

// Component for displaying performance charts
const PerformanceChart = ({ datasets, algorithms }) => {
  const maxExecutionTime = Math.max(
    ...algorithms.flatMap(algo => algo.executionTimes)
  );
  
  const getBarHeight = (time) => {
    const scaleFactor = 300 / maxExecutionTime;
    return Math.max(10, time * scaleFactor);
  };

  const getBarColor = (algorithmName) => {
    const colors = {
      Chiba: '#4CAF50',
      ELS: '#2196F3',
      Tomita: '#F44336',
    };
    return colors[algorithmName] || '#666';
  };

  return (
    <section className="performance">
      <h3>Performance Comparison</h3>
      <div className="performance-container">
        <div className="time-graphs-row">
          {datasets.map((dataset, index) => (
            <div key={dataset.name} className="dataset-performance">
              <h4>{dataset.name}</h4>
              <div className="time-histogram">
                {algorithms.map((algo) => {
                  const time = algo.executionTimes[index];
                  return (
                    <div
                      key={algo.name}
                      className="time-bar"
                      style={{ 
                        height: `${getBarHeight(time)}px`,
                        backgroundColor: getBarColor(algo.name),
                      }}
                    >
                      <span className="time-label">
                        {time.toLocaleString()}s
                      </span>
                      <span className="algo-label">
                        {algo.name}
                      </span>
                    </div>
                  );
                })}
              </div>
            </div>
          ))}
        </div>
      </div>
    </section>
  );
};

// Component for observations
const Observations = () => (
  <section className="observations">
    <h2>Observations</h2>
    <ul>
      <li>ELS performs best with small datasets but scales poorly with large graphs.</li>
      <li>Tomita excels with small graphs but slows significantly with massive datasets like as-Skitter.</li>
      <li>Chiba maintains consistent performance across all dataset sizes.</li>
      <li>All algorithms show increased execution times with larger clique sizes.</li>
    </ul>
  </section>
);

function App() {
  const currentYear = new Date().getFullYear();

  return (
    <div className="App">
      <header>
        <h1>Maximal Clique Enumeration Project</h1>
        <p>Comparing Chiba, ELS, and Tomita Algorithms</p>
      </header>

      <main>
        <section className="project-info">
          <h2>Project Overview</h2>
          <p>
            This project implements and compares three maximal clique enumeration algorithms: Chiba, ELS, and Tomita,
            using C/C++ implementations tested on large graph datasets.
          </p>
        </section>

        <section className="results">
          <h2>Experimental Results</h2>
          <div className="dataset-stats">
            <h3>Dataset Statistics</h3>
            <div className="dataset-grid">
              {PROJECT_DATA.datasets.map((dataset) => (
                <DatasetCard key={dataset.name} dataset={dataset} />
              ))}
            </div>
          </div>
          <PerformanceChart datasets={PROJECT_DATA.datasets} algorithms={PROJECT_DATA.algorithms} />
        </section>

        <Observations />
      </main>

      <footer>
        <p>Created by Soumith | {currentYear}</p>
        <p>
          Source code available on{' '}
          <a href="https://github.com/SSG05/daa_assign" target="_blank" rel="noopener noreferrer">
            GitHub
          </a>
        </p>
      </footer>
    </div>
  );
}

export default App;