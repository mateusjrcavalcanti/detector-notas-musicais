# 🚀 Automação CI/CD - GitHub Actions

Este projeto inclui automação completa para build e release usando GitHub Actions.

## 📋 Como Funciona

### 🔄 Trigger Automático
O workflow é acionado automaticamente quando você cria uma tag de versão:

```bash
# Exemplo: criar e enviar uma tag de versão
git tag v1.0.0
git push origin v1.0.0
```

### 🎯 Trigger Manual
Você também pode executar o workflow manualmente:

1. Vá para a aba "Actions" no GitHub
2. Selecione "Build and Release"
3. Clique em "Run workflow"
4. Digite a versão desejada (ex: v1.0.1)

## 🏗️ O que o Workflow Faz

### 1. **Preparação do Ambiente**
- ✅ Configura Ubuntu Linux
- ✅ Instala dependências (CMake, GCC ARM, Ninja)
- ✅ Baixa e configura o Pico SDK v2.1.1

### 2. **Build do Projeto**
- ✅ Configura o CMake para release
- ✅ Compila o projeto com otimizações
- ✅ Gera todos os formatos de arquivo:
  - `detector-notas-musicais.uf2` (principal)
  - `detector-notas-musicais.elf` (debug)
  - `detector-notas-musicais.bin` (binário)
  - `detector-notas-musicais.hex` (Intel HEX)

### 3. **Criação da Release**
- ✅ Cria uma nova release no GitHub
- ✅ Adiciona descrição detalhada
- ✅ Inclui instruções de instalação
- ✅ Anexa arquivos prontos para download

### 4. **Artefatos Gerados**
- 📁 **Arquivo UF2 individual** - Para instalação direta
- 📦 **Pacote ZIP completo** - Todos os arquivos + documentação
- 🔍 **Build artifacts** - Backup dos arquivos compilados

## 📥 Downloads Disponíveis

Após cada release, os usuários podem baixar:

1. **detector-notas-musicais.uf2** 
   - Arquivo principal para gravação
   - Basta copiar para a placa em modo BOOTSEL

2. **detector-notas-musicais-vX.X.X-complete.zip**
   - Pacote completo com todos os formatos
   - Inclui documentação e instruções
   - Ideal para desenvolvedores

## 🎯 Padrão de Versionamento

Use [Semantic Versioning](https://semver.org/lang/pt-BR/):

- **v1.0.0** - Release principal
- **v1.0.1** - Correção de bugs
- **v1.1.0** - Novas funcionalidades
- **v2.0.0** - Mudanças que quebram compatibilidade

## 🛠️ Para Desenvolvedores

### Testando Localmente

Antes de criar uma release, teste o build localmente:

```bash
# Limpar build anterior
rm -rf build

# Configurar e compilar
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DPICO_BOARD=pico_w
make -j$(nproc)

# Verificar arquivos gerados
ls -la *.uf2 *.elf *.bin *.hex
```

### Criando uma Nova Release

1. **Teste local completo**
2. **Commit todas as mudanças**
3. **Crie e envie a tag:**
   ```bash
   git tag v1.0.0
   git push origin v1.0.0
   ```
4. **Monitore o workflow** na aba Actions
5. **Verifique a release** criada automaticamente

## 🔧 Customização

Para modificar o workflow, edite `.github/workflows/build-and-release.yml`:

- **Dependências:** Seção `Install dependencies`
- **Build:** Seção `Configure CMake` e `Build project`
- **Artefatos:** Seção `Create release archive`
- **Release notes:** Seção `Create Release`

## 📊 Status do Build

[![Build and Release](https://github.com/seu-usuario/detector-notas-musicais/actions/workflows/build-and-release.yml/badge.svg)](https://github.com/seu-usuario/detector-notas-musicais/actions/workflows/build-and-release.yml)

*Substitua `seu-usuario` pelo seu nome de usuário do GitHub*

---

## 🆘 Resolução de Problemas

### Build Falhando
- Verifique se todas as dependências estão corretas
- Confirme que o código compila localmente
- Veja os logs detalhados na aba Actions

### Release Não Criada
- Verifique se a tag foi enviada corretamente
- Confirme que o token GITHUB_TOKEN tem permissões
- Verifique se não há conflitos de nomes de tag

### Arquivos Ausentes
- Confirme que o build foi bem-sucedido
- Verifique se os caminhos dos arquivos estão corretos
- Veja se todos os formatos estão sendo gerados
