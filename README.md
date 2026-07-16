# Conversor

Conversor é um aplicativo desktop para converter arquivos localmente, com foco em privacidade, conversões em lote e escolhas seguras de qualidade.

O programa foi pensado para Windows, Linux e macOS usando uma interface nativa em Qt/C++. A tela principal segue um estilo clássico de utilitário desktop, com grupos de configuração, tabela de arquivos, destino, opções de conversão e status em um fluxo direto. Os arquivos são processados no próprio computador; nada é enviado para serviços externos.

## Baixar o programa

Se você só quer usar o Conversor no Windows, baixe a release mais recente:

| Sistema | Download direto |
| --- | --- |
| Windows | [Baixar instalador MSI](https://github.com/leonfagundes/Conversor-/releases/latest/download/Conversor-Windows-x64.msi) |
| Windows | [Baixar pacote ZIP portátil](https://github.com/leonfagundes/Conversor-/releases/latest/download/Conversor-Windows-x64.zip) |
| Todos os downloads | [Abrir página de releases](https://github.com/leonfagundes/Conversor-/releases) |

Linux e macOS continuam planejados, mas ainda não têm artefatos publicados na release atual. Quando forem publicados, eles devem aparecer na página de releases com estes nomes:

- `Conversor-Linux-x86_64.AppImage`
- `Conversor-macOS-universal.dmg`

Depois de baixar, abra o arquivo e siga o fluxo normal do seu sistema operacional.

## Imagens do programa

### Tela principal

![Tela principal do Conversor](docs/images/conversor-tela-principal.png)

### Fila de conversão

![Fila de conversão no Conversor](docs/images/conversor-fila-conversao.png)

### Tema escuro

![Tela principal do Conversor em tema escuro](docs/images/conversor-tela-principal-dark.png)

## O que o Conversor faz

- Converte arquivos de áudio, vídeo, imagem e documentos.
- Permite adicionar arquivos avulsos ou pastas inteiras.
- Organiza a conversão por categoria de arquivo.
- Prioriza formatos que preservam qualidade quando isso é possível.
- Mostra avisos quando o formato escolhido pode causar perda de qualidade.
- Segue o tema claro/escuro do sistema quando ele é detectado e permite escolher o tema dentro do programa.
- Procura motores empacotados junto do aplicativo antes de usar programas disponíveis no `PATH`.

## Estado atual do projeto

Este repositório contém a primeira base do projeto:

- Núcleo em C++ puro para detectar categorias, listar formatos de destino e planejar conversões.
- Testes automatizados do núcleo com CTest.
- Interface desktop em Qt Widgets com layout clássico em grupos, tabela de arquivos, abas por categoria, opções de saída, avisos, progresso e log.
- Montadores de comandos para FFmpeg, ImageMagick, LibreOffice e Pandoc.
- Notas de empacotamento para Windows, Linux e macOS.

## Requisitos para desenvolvimento

- CMake 3.24 ou mais recente
- Compilador com suporte a C++20
- Qt 6 Widgets para compilar o aplicativo desktop

O núcleo e os testes não dependem de Qt. Se o Qt 6 Widgets não estiver instalado, o CMake ainda compila o núcleo e os testes, mas ignora o executável desktop.

## Compilar localmente

```powershell
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

## Estrutura do projeto

```text
src/core/      Lógica de planejamento de conversão em C++ puro
src/app/       Interface desktop em Qt Widgets
tests/         Testes do núcleo
docs/          Documentação do produto e da arquitetura
packaging/     Notas de empacotamento por plataforma
```

## Privacidade

O Conversor foi projetado para funcionar 100% localmente. Os motores de conversão devem ser empacotados junto de cada build, e o aplicativo não precisa de internet para processar arquivos.
