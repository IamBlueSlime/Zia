# Zia ![Test](https://github.com/IamBlueSlime/Zia/workflows/Test/badge.svg) [![License](https://img.shields.io/badge/license-MIT-blue.svg?style=flat-square)](LICENSE.txt)

## Introduction

**Zia** is an Epitech school project. The main objective is to create an HTTP
web server in C++. All specific features has to be implemented in a separate
module. This way, we can select which feature to enable by placing the module
library in the _modules_ folder.

We has to implement a common API to permits to use the modules of another
group in our server implementation.

Implemented modules:
- [Authorization](https://github.com/IamBlueSlime/Zia/tree/master/modules/authorization): manage HTTP basic auth
- [CGI](https://github.com/IamBlueSlime/Zia/tree/master/modules/cgi): manage binaries which create HTTP responses
- [Error](https://github.com/IamBlueSlime/Zia/tree/master/modules/error): create generic error pages
- [Head](https://github.com/IamBlueSlime/Zia/tree/master/modules/head): manage HEAD HTTP method
- [Headers](https://github.com/IamBlueSlime/Zia/tree/master/modules/headers): parse HTTP headers, query params and body
- [HTTP](https://github.com/IamBlueSlime/Zia/tree/master/modules/http): manage _HTTP_ requests and responses
- [LS](https://github.com/IamBlueSlime/Zia/tree/master/modules/ls): show a file browser when the requested resource is a folder
- [Raw](https://github.com/IamBlueSlime/Zia/tree/master/modules/raw): returns the requested file as raw if none of the modules proceeded the request
- [SSL](https://github.com/IamBlueSlime/Zia/tree/master/modules/ssl): manage _HTTPS_ requests and responses

**We don't plan to continue this project, we just published it as part of
our portfolios.**

## Installation

We use **Conan** as a dependency manager and **CMake** to build our project.

```
$ mkdir build
$ cd build
$ conan install ..
$ cmake ..
$ make
```

You have to create the `config` and `modules` folders in the same path as
the binary. Place the libraries in the `modules` folder. You can find our
configurations files [here](https://github.com/IamBlueSlime/Zia/tree/master/config).

## License

See [LICENSE](LICENSE).


## Authors

- Jérémy "IamBlueSlime" Levilain - [GitHub](https://github.com/IamBlueSlime) - [Website](https://jeremylvln.fr) - [Twitter](https://twitter.com/iamblueslime)
- Gwendal Dumail - [GitHub](https://github.com/GDumail)
- Juel Marques-Garcia - [GitHub](https://github.com/FriendlyManiakk)
- Théo Zapata - [GitHub](https://github.com/Thezap)
- Laura Berthe-Ribeiro - [GitHub](https://github.com/LoloRibeiro)