#include<vector>
#include<cstdlib>
#include<random>
#include<algorithm>

struct SamplingDataSet
{
public:
  std::vector<float> data;
  std::uniform_int_distribution<size_t> sample_distribution;
  SamplingDataSet(size_t tN=10000,unsigned tseed=42):data(tN),sample_distribution(0,tN-1)
  {
    std::default_random_engine rengine(tseed);
    std::normal_distribution<float> data_distribution(5.0f,2.0f);
    std::generate(data.begin(),data.end(),[&]{
      return data_distribution(rengine);
    });
  }
};

static const unsigned S=42;
static const size_t N=2000000;
static const size_t T=N/100;
static SamplingDataSet dataset(N,S);

static void NaiveSampleMean(benchmark::State& state) {
  std::default_random_engine rengine(S);

  for (auto _ : state) {
    float mu=0.0f;
    float fiT=1.0f/static_cast<float>(T);
    for(size_t i=0;i<T;i++)
    {
      size_t index=dataset.sample_distribution(rengine);
      mu+=dataset.data[index]*fiT;
    }
    benchmark::DoNotOptimize(mu);
  }
}
// Register the function as a benchmark
BENCHMARK(NaiveSampleMean);

static void PrecomputeLocations(benchmark::State& state) {
  std::default_random_engine rengine(S);
  std::vector<size_t> locations(T);
  std::generate(locations.begin(),locations.end(),[&]{
      return dataset.sample_distribution(rengine);
    });
  for (auto _ : state) {
    float mu=0.0f;
    float fiT=1.0f/static_cast<float>(T);
    for(size_t i=0;i<T;i++)
    {
      size_t index=locations[i];
      mu+=dataset.data[index]*fiT;
    }
    benchmark::DoNotOptimize(mu);
  }
}
BENCHMARK(PrecomputeLocations);

static void PrecomputeSortLocations(benchmark::State& state) {
  std::default_random_engine rengine(S);
  std::vector<size_t> locations(T);
  std::generate(locations.begin(),locations.end(),[&]{
      return dataset.sample_distribution(rengine);
    });
  std::sort(locations.begin(),locations.end());
  for (auto _ : state) {
    float mu=0.0f;
    float fiT=1.0f/static_cast<float>(T);
    for(size_t i=0;i<T;i++)
    {
      size_t index=locations[i];
      mu+=dataset.data[index]*fiT;
    }
    benchmark::DoNotOptimize(mu);
  }
}
BENCHMARK(PrecomputeSortLocations);

