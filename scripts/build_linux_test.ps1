$config_type = 'Debug'

Set-Location $PSScriptRoot\..\test  # enter the parent folder
cmake --build .\$config_type --config $config_type  # build the project
