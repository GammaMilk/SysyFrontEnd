# SysyFrontEnd

## Brief

- [x] lex & parser
- [x] code generation
- [ ] optimization

## Usage

### Clone and compile

```bash
git clone https://github.com/GammaMilk/SysyFrontEnd.git
cd SysyFrontEnd
mkdir build&&cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j8
```

### Use the compiler(Now it can only generate LLVMIR code)

```bash
./SysyLex -S <SourceFilePath> [-O1|-O2|-O0] [-arm] [-dso] -o <OutputFilePath>
```

Explanation:

- The `-Ox` option is used to specify the optimization level of the compiler.
- The `-arm` option is used to generate the `arm_aapcs_vfpcc` attribute in the Intermediate Representation (IR). The
  option will be ignored while generating ASM.
- The `-dso` option is used to generate the `dso_local` attribute in the Intermediate Representation (IR). The option
  will be ignored while generating ASM.

## ⚠ Attention ⚠

This project is just one part of
a [competition about compilers](https://cscc.educg.net/#/index?name=2023%E5%85%A8%E5%9B%BD%E5%A4%A7%E5%AD%A6%E7%94%9F%E8%AE%A1%E7%AE%97%E6%9C%BA%E7%B3%BB%E7%BB%9F%E8%83%BD%E5%8A%9B%E5%A4%A7%E8%B5%9B%E7%BC%96%E8%AF%91%E7%B3%BB%E7%BB%9F%E8%AE%BE%E8%AE%A1%E8%B5%9B).
Maintained personally by the [author](https://github.com/Gaomengkai). Except for the participants, the author does not
provide any technical support, nor does it guarantee that the project can run on any platform.

## Milestone

- 20230718 This sysy-frontend can pass all the 140 functional tests provided by educg.net.