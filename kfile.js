const fs = require('fs');

let project = new Project('LabS11');

project.kore = true;
project.cmd = true;

project.addIncludeDir('optick/include')
project.addExclude('optick/src/optick_gpu.vulkan.cpp')
project.addExclude('optick/src/optick_gpu.d3d12.cpp')
project.addFiles('Sources/**','optick/src/**');
project.setDebugDir('./Deployment');


resolve(project);
