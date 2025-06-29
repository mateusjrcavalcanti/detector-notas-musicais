#!/bin/bash

# Script para criar uma nova release do Detector de Notas Musicais
# Uso: ./scripts/create-release.sh v1.0.0

set -e  # Para na primeira falha

# Cores para output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Função para imprimir mensagens coloridas
print_info() {
    echo -e "${BLUE}ℹ️  $1${NC}"
}

print_success() {
    echo -e "${GREEN}✅ $1${NC}"
}

print_warning() {
    echo -e "${YELLOW}⚠️  $1${NC}"
}

print_error() {
    echo -e "${RED}❌ $1${NC}"
}

# Verificar se a versão foi fornecida
if [ $# -eq 0 ]; then
    print_error "Versão não especificada!"
    echo "Uso: $0 <versão>"
    echo "Exemplo: $0 v1.0.0"
    exit 1
fi

VERSION=$1

# Validar formato da versão
if [[ ! $VERSION =~ ^v[0-9]+\.[0-9]+\.[0-9]+$ ]]; then
    print_error "Formato de versão inválido!"
    echo "Use o formato: vX.Y.Z (exemplo: v1.0.0)"
    exit 1
fi

print_info "Criando release $VERSION"

# Verificar se estamos no diretório correto
if [ ! -f "CMakeLists.txt" ] || [ ! -f "main.c" ]; then
    print_error "Execute este script na raiz do projeto!"
    exit 1
fi

# Verificar se git está limpo
if [ -n "$(git status --porcelain)" ]; then
    print_warning "Há arquivos não commitados!"
    git status --short
    read -p "Continuar mesmo assim? (y/N): " -n 1 -r
    echo
    if [[ ! $REPLY =~ ^[Yy]$ ]]; then
        print_info "Operação cancelada."
        exit 1
    fi
fi

# Verificar se a tag já existe
if git tag -l | grep -q "^$VERSION$"; then
    print_error "Tag $VERSION já existe!"
    print_info "Tags existentes:"
    git tag -l | tail -5
    exit 1
fi

# Teste de build local
print_info "Testando build local..."

# Limpar build anterior
if [ -d "build" ]; then
    print_info "Limpando build anterior..."
    rm -rf build
fi

# Criar e configurar build
print_info "Configurando CMake..."
mkdir build
cd build

if ! cmake .. -DCMAKE_BUILD_TYPE=Release -DPICO_BOARD=pico_w; then
    print_error "Falha na configuração do CMake!"
    exit 1
fi

# Compilar
print_info "Compilando projeto..."
if ! make -j$(nproc); then
    print_error "Falha na compilação!"
    exit 1
fi

# Verificar arquivos gerados
print_info "Verificando arquivos gerados..."
required_files=("detector-notas-musicais.uf2" "detector-notas-musicais.elf" "detector-notas-musicais.bin" "detector-notas-musicais.hex")

for file in "${required_files[@]}"; do
    if [ -f "$file" ]; then
        print_success "$file encontrado"
    else
        print_error "$file não encontrado!"
        exit 1
    fi
done

cd ..

print_success "Build local bem-sucedido!"

# Mostrar estatísticas dos arquivos
print_info "Estatísticas dos arquivos:"
ls -lh build/detector-notas-musicais.*

# Confirmar criação da release
echo
print_warning "Pronto para criar release $VERSION"
echo "Isso irá:"
echo "  1. Criar tag local $VERSION"
echo "  2. Enviar tag para o GitHub"
echo "  3. Acionar o GitHub Actions automaticamente"
echo "  4. Criar release com arquivos prontos"
echo
read -p "Continuar? (y/N): " -n 1 -r
echo

if [[ ! $REPLY =~ ^[Yy]$ ]]; then
    print_info "Operação cancelada."
    exit 0
fi

# Criar e enviar tag
print_info "Criando tag $VERSION..."
git tag -a "$VERSION" -m "Release $VERSION

Arquivos inclusos:
- detector-notas-musicais.uf2 (arquivo principal para gravação)
- detector-notas-musicais.elf (executável com símbolos)
- detector-notas-musicais.bin (imagem binária)
- detector-notas-musicais.hex (formato Intel HEX)

Build automático via GitHub Actions."

print_info "Enviando tag para o GitHub..."
git push origin "$VERSION"

print_success "Tag $VERSION enviada com sucesso!"

print_info "GitHub Actions foi acionado automaticamente."
print_info "Acompanhe o progresso em: https://github.com/$(git config --get remote.origin.url | sed 's/.*github.com[:/]\([^.]*\).*/\1/')/actions"

print_success "Release $VERSION criada com sucesso!"
print_info "A release estará disponível em alguns minutos na página de releases do GitHub."
