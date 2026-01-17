# MCP (Model Context Protocol) セットアップガイド

## 概要

本プロジェクトでは**UnrealGenAISupport**プラグインを使用し、Claude CodeおよびClaude DesktopからUnreal Engineを制御します。

---

## 1. 前提条件

### 必要なソフトウェア
- Unreal Engine 5.7
- Python 3.10+
- Node.js 18+ (MCP Server用)
- Claude Desktop または Claude Code

### APIキー
以下の環境変数を設定：

```bash
# Windows (PowerShell)
$env:PS_ANTHROPICAPIKEY = "your-anthropic-api-key"

# または システム環境変数に設定
setx PS_ANTHROPICAPIKEY "your-anthropic-api-key"
```

---

## 2. プラグインセットアップ

### 2.1 Git Submoduleの初期化

新規クローン時：
```bash
git clone --recurse-submodules <repository-url>
```

既存リポジトリ：
```bash
git submodule update --init --recursive
```

### 2.2 プラグインの確認

`Dawnlight/Plugins/UnrealGenAISupport/` にプラグインが存在することを確認。

### 2.3 UE5での有効化

1. プロジェクトを開く
2. Edit → Plugins
3. "GenerativeAISupport" を検索
4. 有効化されていることを確認
5. エディタを再起動

---

## 3. Unreal MCP Server セットアップ

### 3.1 MCP依存関係のインストール

```bash
pip install "mcp[cli]"
pip install mss
```

### 3.2 Claude Desktop設定

`%APPDATA%\Claude\claude_desktop_config.json` を編集：

```json
{
  "mcpServers": {
    "unreal-handshake": {
      "command": "python",
      "args": [
        "D:/PersonalGameDev/One-Night-One-Loop/Dawnlight/Plugins/UnrealGenAISupport/Content/Python/mcp_server.py"
      ],
      "env": {
        "UNREAL_HOST": "localhost",
        "UNREAL_PORT": "9877"
      }
    }
  }
}
```

### 3.3 Claude Code設定

`.claude/settings.json`（プロジェクトルートに作成済み）：

```json
{
  "mcpServers": {
    "unreal-handshake": {
      "command": "python",
      "args": [
        "D:/PersonalGameDev/One-Night-One-Loop/Dawnlight/Plugins/UnrealGenAISupport/Content/Python/mcp_server.py"
      ],
      "env": {
        "UNREAL_HOST": "localhost",
        "UNREAL_PORT": "9877"
      }
    }
  }
}
```

### 3.4 Cursor IDE設定

`.cursor/mcp.json`（プロジェクトルートに作成）：

```json
{
  "mcpServers": {
    "unreal-handshake": {
      "command": "python",
      "args": [
        "D:/PersonalGameDev/One-Night-One-Loop/Dawnlight/Plugins/UnrealGenAISupport/Content/Python/mcp_server.py"
      ],
      "env": {
        "UNREAL_HOST": "localhost",
        "UNREAL_PORT": "9877"
      }
    }
  }
}
```

---

## 4. 使用可能なMCPツール

### 4.1 シーン操作

| ツール | 説明 | 例 |
|--------|------|-----|
| `spawn_actor` | アクターを生成 | 光源、カメラ、トリガーボックス |
| `move_actor` | アクターを移動 | 位置調整 |
| `delete_actor` | アクターを削除 | クリーンアップ |
| `set_material` | マテリアルを変更 | 外観調整 |

### 4.2 Blueprint操作

| ツール | 説明 | 例 |
|--------|------|-----|
| `create_blueprint` | BPを生成 | イベントBP作成 |
| `add_component` | コンポーネント追加 | ライト、コリジョン |
| `connect_nodes` | ノードを接続 | ロジック構築 |

### 4.3 Pythonスクリプト実行

| ツール | 説明 | 例 |
|--------|------|-----|
| `run_python` | Pythonスクリプト実行 | エディタ自動化 |
| `run_editor_utility` | エディタユーティリティ実行 | バッチ処理 |

---

## 5. Night 1での活用シナリオ

### 5.1 光源配置の自動化

```
Claude: 「Night 1のメインホールに、3秒間隔で点滅する光源を5つ配置して」

MCP Server:
1. spawn_actor で PointLight を5つ生成
2. 各ライトにランダムな位置を設定
3. Blueprint で点滅ロジックを追加
```

### 5.2 イベントトリガー配置

```
Claude: 「プレイヤーが近づくと監視レベルが上がるトリガーを設置」

MCP Server:
1. spawn_actor で TriggerBox を生成
2. OnOverlap イベントを設定
3. SurveillanceSubsystem への通知を接続
```

### 5.3 プレイテスト分析

```
Claude: 「最新のプレイログを分析して、死亡が多い場所を教えて」

処理:
1. Saved/Logs/PlayLog.json を読み込み
2. 死亡位置を集計
3. ヒートマップ情報を返却
```

---

## 6. プレイログ出力設定

### 6.1 ログフォーマット

```json
{
  "session_id": "uuid",
  "timestamp": "2025-01-15T10:30:00Z",
  "events": [
    {
      "type": "player_death",
      "position": {"x": 100, "y": 200, "z": 0},
      "cause": "detected",
      "phase": "climax",
      "tension_level": 0.85
    },
    {
      "type": "photograph_taken",
      "position": {"x": 150, "y": 250, "z": 0},
      "target": "evidence_001",
      "success": true
    }
  ]
}
```

### 6.2 ログカテゴリ

| カテゴリ | イベント |
|---------|---------|
| `player` | death, hide, photograph |
| `surveillance` | detection_increase, alert_triggered |
| `event` | fixed_event_triggered, random_event_selected |
| `phase` | phase_transition |

---

## 7. トラブルシューティング

### MCP Serverに接続できない

1. Pythonパスを確認
2. 環境変数が設定されているか確認
3. UE5エディタが起動しているか確認

### APIキーエラー

1. `PS_ANTHROPICAPIKEY` が正しく設定されているか確認
2. APIキーの有効期限を確認

### プラグインが認識されない

1. プロジェクトファイルを再生成
2. エディタを完全に再起動
3. `.uproject` の Plugins セクションを確認

---

## 8. セキュリティ注意事項

- APIキーは`.env`ファイルまたは環境変数で管理
- **絶対にAPIキーをGitにコミットしない**
- `.gitignore` に以下を追加済み：
  ```
  .env
  .env.*
  ```

---

## 参考リンク

- [UnrealGenAISupport GitHub](https://github.com/prajwalshettydev/UnrealGenAISupport)
- [MCP Protocol Documentation](https://modelcontextprotocol.io/)
- [Claude API Documentation](https://docs.anthropic.com/)

---

**Last Updated**: 2025-01-15
