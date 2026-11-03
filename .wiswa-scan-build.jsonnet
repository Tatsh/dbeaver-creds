{
  checkout_options: { 'fetch-depth': 0 },
  configure_command: 'cmake -S . -B build-scan -DCMAKE_BUILD_TYPE=Debug -DBUILD_PYTHON_EXTENSION=ON',
  documentation_steps: [
    {
      name: 'Install dependencies',
      run: 'sudo apt-get update\nsudo apt-get install -y doxygen graphviz\n',
    },
    {
      name: 'Configure',
      run: 'cmake -DCMAKE_BUILD_TYPE=Release -B "$GITHUB_WORKSPACE/build" -DBUILD_DOCS=ON',
    },
    {
      name: 'Build documentation',
      run: 'cmake --build "$GITHUB_WORKSPACE/build" --target docs --verbose',
    },
  ],
  env: {},
  setup_steps: [
    {
      name: 'Install build dependencies',
      run: 'sudo apt-get update\nsudo apt-get install -y libssl-dev python3-dev\n',
    },
    {
      name: 'Install current CMake',
      run: 'pipx install cmake',
    },
  ],
  workflow_file: 'pages.yml',
}
