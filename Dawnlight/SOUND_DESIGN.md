# Dawnlight - Sound Design Document

## Night 1 Demo Edition

**Version**: 0.1.0
**Last Updated**: 2025-01-16

---

## 1. サウンドコンセプト

### 1.1 キーワード
- **静寂と音** - 静けさが音を際立たせる
- **監視の気配** - 機械的なハム音、電子音
- **孤独** - 反響する足音
- **緊張** - 心拍、呼吸
- **予兆** - 不安を煽る環境音

### 1.2 参考作品

| 作品 | 参考要素 |
|------|----------|
| **Alien: Isolation** | 監視システム音、緊張感のある環境音 |
| **Inside** | ミニマルな効果音、無音の使い方 |
| **Resident Evil 7** | 環境音による恐怖演出 |
| **Dead Space** | 機械音、金属音の反響 |
| **Outlast** | カメラ音、暗闘での聴覚依存 |

### 1.3 サウンドテーマ: Night 1

> 「静けさの中で、自分の存在が音になる」

- プレイヤーの行動が音を生む → リスク
- 環境音が「監視されている」感覚を強化
- 無音の瞬間が最も緊張する

---

## 2. サウンドカテゴリ

### 2.1 カテゴリ構成

```
Audio/
├── SFX/                    # 効果音
│   ├── Player/             # プレイヤー関連
│   ├── Environment/        # 環境音
│   ├── UI/                 # UI効果音
│   └── Events/             # イベント効果音
│
├── Ambience/               # 環境アンビエンス
│   ├── Base/               # ベースアンビエンス
│   ├── Layers/             # レイヤー音
│   └── OneShots/           # ランダムワンショット
│
└── Music/                  # 音楽（最小限）
    ├── Stingers/           # スティンガー（短い演出音）
    └── Themes/             # テーマ曲
```

---

## 3. 効果音設計

### 3.1 プレイヤーSFX

#### 足音

| 表面 | 特徴 | 検知リスク | ファイル名 |
|------|------|-----------|-----------|
| **コンクリート** | 硬い、反響 | 高 | S_Footstep_Concrete_* |
| **金属** | 金属音、響く | 高 | S_Footstep_Metal_* |
| **タイル** | クリア、エコー | 中 | S_Footstep_Tile_* |
| **カーペット** | 柔らかい、静か | 低 | S_Footstep_Carpet_* |
| **水たまり** | 水音 | 高 | S_Footstep_Water_* |

```cpp
// 足音の実装パラメータ
struct FFootstepParams
{
    float VolumeMultiplier;     // 表面による音量
    float DetectionRadius;      // 検知半径（メートル）
    float PitchVariation;       // ピッチ変動範囲
    int32 VariationCount;       // バリエーション数
};

// 例: コンクリート
Concrete.VolumeMultiplier = 1.0f;
Concrete.DetectionRadius = 15.0f;
Concrete.PitchVariation = 0.1f;
Concrete.VariationCount = 4;

// 例: カーペット
Carpet.VolumeMultiplier = 0.3f;
Carpet.DetectionRadius = 5.0f;
Carpet.PitchVariation = 0.15f;
Carpet.VariationCount = 4;
```

#### 撮影音

| 音 | タイミング | 検知リスク | ファイル名 |
|-----|-----------|-----------|-----------|
| **シャッター音** | 撮影実行時 | 高 | S_Camera_Shutter |
| **フォーカス音** | 撮影準備時 | 低 | S_Camera_Focus |
| **フィルム巻き上げ** | 撮影後 | 中 | S_Camera_Wind |

```cpp
// シャッター音のリスク設計
ShutterSound.DetectionRadius = 20.0f;  // 20m範囲で検知リスク
ShutterSound.AlertIncrease = 15.0f;    // 監視レベル+15%
```

#### 隠れる音

| 音 | タイミング | 検知リスク | ファイル名 |
|-----|-----------|-----------|-----------|
| **潜り込み** | 隠れ開始 | 中 | S_Hide_Enter |
| **衣擦れ** | 隠れ中（ランダム） | 低 | S_Hide_Rustle |
| **出る音** | 隠れ終了 | 中 | S_Hide_Exit |

#### 呼吸・心拍

| 状態 | 特徴 | ファイル名 |
|------|------|-----------|
| **通常** | 静かな呼吸 | S_Breath_Normal |
| **緊張** | 荒い呼吸 | S_Breath_Tense |
| **検知状態** | 心拍 + 荒い呼吸 | S_Breath_Alert |
| **隠れ中** | 息を殺す | S_Breath_Hiding |

```cpp
// 呼吸音の緊張度連動
void UpdateBreathSound(float TensionLevel)
{
    // TensionLevel: 0.0 - 1.0
    if (TensionLevel < 0.3f)
        PlayBreathSound(EBreathType::Normal);
    else if (TensionLevel < 0.7f)
        PlayBreathSound(EBreathType::Tense);
    else
        PlayBreathSound(EBreathType::Alert);
}
```

### 3.2 環境SFX

#### 監視システム音

| 音 | 説明 | ループ | ファイル名 |
|-----|------|--------|-----------|
| **カメラ回転** | サーボモーター音 | No | S_Camera_Rotate |
| **カメラ停止** | クリック音 | No | S_Camera_Stop |
| **スポットライト起動** | 電気音 + ハム | No | S_Spotlight_On |
| **スポットライト移動** | 機械音 | No | S_Spotlight_Move |
| **スポットライト停止** | 減速音 | No | S_Spotlight_Stop |
| **システムハム** | 電子機器の低音 | Yes | S_System_Hum_Loop |

#### 環境音

| 音 | 説明 | トリガー | ファイル名 |
|-----|------|---------|-----------|
| **蛍光灯点滅** | バチバチ音 | ランダム | S_Light_Flicker |
| **配管音** | 水流、金属膨張 | ランダム | S_Pipe_Creak |
| **換気扇** | 回転音 | エリア | S_Vent_Fan_Loop |
| **電気スパーク** | 壊れた機器 | トリガー | S_Electric_Spark |
| **ドア軋み** | 自然発生 | ランダム | S_Door_Creak |
| **遠くの金属音** | 不明な音源 | ランダム | S_Distant_Metal |

### 3.3 UI SFX

| 音 | 説明 | ファイル名 |
|-----|------|-----------|
| **検知ゲージ上昇** | 電子音、上昇 | S_UI_Detection_Up |
| **検知ゲージ警告** | ビープ音、速い | S_UI_Detection_Warning |
| **検知ゲージMAX** | アラーム | S_UI_Detection_Max |
| **撮影成功** | 確認音 | S_UI_Photo_Success |
| **ポーズ開始** | ソフトなクリック | S_UI_Pause_On |
| **ポーズ終了** | ソフトなクリック | S_UI_Pause_Off |

---

## 4. アンビエンス設計

### 4.1 ベースアンビエンス

**Night 1 基本アンビエンス構成**

```
Layer 1: Foundation (常に再生)
├─ 低周波ハム（空調、電力）
├─ 遠くの機械音
└─ 建物の「呼吸」

Layer 2: Area-Specific (エリアごと)
├─ 入口: 外気の微かな音
├─ 通路: 反響、閉塞感
├─ メインホール: 広い空間の残響
└─ 深部: より静か、圧迫感

Layer 3: Dynamic (状況に応じて)
├─ 緊張度による変化
├─ フェーズによる変化
└─ イベントトリガー
```

### 4.2 エリア別アンビエンス

| エリア | 特徴 | ベースdB | ファイル名 |
|--------|------|---------|-----------|
| **入口** | 外界との境界 | -18dB | S_Amb_Entrance |
| **通路A/B** | 閉塞感、反響 | -20dB | S_Amb_Corridor |
| **中央ホール** | 広い空間、残響 | -22dB | S_Amb_MainHall |
| **小部屋** | 静か、密閉 | -24dB | S_Amb_SmallRoom |
| **深部** | 最も静か | -26dB | S_Amb_DeepArea |

### 4.3 ランダムワンショット

```cpp
// ランダム環境音の発生システム
struct FRandomSoundConfig
{
    float MinInterval;      // 最小間隔（秒）
    float MaxInterval;      // 最大間隔（秒）
    float MaxDistance;      // 発生距離（プレイヤーから）
    TArray<USoundCue*> SoundPool;  // サウンドプール
};

// 設定例
RandomSounds.MinInterval = 15.0f;
RandomSounds.MaxInterval = 45.0f;
RandomSounds.MaxDistance = 30.0f;
RandomSounds.SoundPool = {
    S_Distant_Metal,
    S_Pipe_Creak,
    S_Door_Creak,
    S_Vent_Rattle
};
```

---

## 5. 音楽設計

### 5.1 Night 1 の音楽方針

**最小限のアプローチ**
- 常時BGMは使用しない
- スティンガー（短い演出音）を使用
- 無音が緊張を生む

### 5.2 スティンガー

| スティンガー | トリガー | 長さ | ファイル名 |
|-------------|---------|------|-----------|
| **最初の光** | イベント1開始 | 3秒 | S_Stinger_FirstLight |
| **残された記録** | イベント2発見時 | 4秒 | S_Stinger_Discovery |
| **覚醒する監視** | イベント3開始 | 5秒 | S_Stinger_Awakening |
| **検知された** | 監視レベルMAX | 2秒 | S_Stinger_Detected |
| **夜明け** | デモ終了 | 8秒 | S_Stinger_Dawn |

### 5.3 緊張度レイヤー

```cpp
// 緊張度に応じたアンビエンスレイヤー
void UpdateTensionAudio(float TensionLevel)
{
    // 0.0 - 0.3: 静か
    // 0.3 - 0.6: 低音ドローン追加
    // 0.6 - 0.8: 不協和音追加
    // 0.8 - 1.0: 心拍、ノイズ追加

    TensionDrone.Volume = FMath::Lerp(0.0f, 0.5f, TensionLevel);
    DissonanceLayer.Volume = FMath::Max(0.0f, (TensionLevel - 0.3f) * 1.5f);
    HeartbeatLayer.Volume = FMath::Max(0.0f, (TensionLevel - 0.6f) * 2.5f);
}
```

---

## 6. 空間オーディオ

### 6.1 リバーブ設定

| エリア | リバーブタイプ | Decay Time | 特徴 |
|--------|--------------|------------|------|
| **入口** | Small Room | 0.8s | 軽い反響 |
| **通路** | Corridor | 1.2s | 長い、狭い |
| **メインホール** | Large Hall | 2.5s | 広い、残響 |
| **小部屋** | Closet | 0.4s | 詰まった音 |
| **深部** | Bunker | 1.8s | 重い、暗い |

### 6.2 オクルージョン

```cpp
// 壁・障害物による音の減衰
struct FOcclusionSettings
{
    float WallAttenuation;      // 壁による減衰（dB）
    float DoorAttenuation;      // ドアによる減衰（dB）
    float DistanceAttenuation;  // 距離による減衰曲線
};

OcclusionSettings.WallAttenuation = -12.0f;  // 壁1枚で-12dB
OcclusionSettings.DoorAttenuation = -6.0f;   // ドアで-6dB
```

### 6.3 3Dサウンド設定

```cpp
// 3Dサウンドの減衰設定
AttenuationSettings.DistanceMin = 100.0f;    // 100cm以内は最大音量
AttenuationSettings.DistanceMax = 3000.0f;   // 30mで聞こえなくなる
AttenuationSettings.AttenuationShape = EAttenuationShape::Sphere;
AttenuationSettings.FalloffDistance = 2000.0f;
```

---

## 7. 検知システムと音

### 7.1 音の検知半径

| 行動 | 検知半径 | 監視レベル増加 |
|------|---------|---------------|
| **歩行（コンクリート）** | 15m | +2%/秒 |
| **歩行（カーペット）** | 5m | +0.5%/秒 |
| **走行** | 25m | +5%/秒 |
| **撮影（シャッター）** | 20m | +15%（即時） |
| **隠れる（入る）** | 10m | +5%（即時） |
| **物を落とす** | 20m | +10%（即時） |

### 7.2 聴覚フィードバック

```cpp
// プレイヤーへの聴覚フィードバック
void OnDetectionIncreased(float NewLevel, float PreviousLevel)
{
    float Delta = NewLevel - PreviousLevel;

    if (Delta > 20.0f)
    {
        // 大きな検知増加 → 警告音
        PlaySound(S_UI_Detection_Warning);
    }
    else if (Delta > 10.0f)
    {
        // 中程度の検知増加 → 小さな音
        PlaySound(S_UI_Detection_Up);
    }

    // 80%以上で心拍音開始
    if (NewLevel >= 80.0f && PreviousLevel < 80.0f)
    {
        StartHeartbeatLoop();
    }
}
```

---

## 8. 技術仕様

### 8.1 オーディオフォーマット

| 用途 | フォーマット | サンプルレート | ビット深度 |
|------|-------------|---------------|-----------|
| **効果音** | WAV | 48kHz | 16bit |
| **アンビエンス** | OGG | 48kHz | 16bit |
| **音楽/スティンガー** | OGG | 48kHz | 24bit |

### 8.2 バス構成

```
Master Bus
├── SFX Bus
│   ├── Player Bus (足音、撮影、呼吸)
│   ├── Environment Bus (環境効果音)
│   └── UI Bus (UI効果音)
│
├── Ambience Bus
│   ├── Base Ambience Bus
│   └── Dynamic Ambience Bus
│
└── Music Bus
    ├── Stinger Bus
    └── Tension Layer Bus
```

### 8.3 Sound Cue 設計

```cpp
// バリエーション付きSoundCue構造
SoundCue_Footstep_Concrete
├── Randomize Node
│   ├── S_Footstep_Concrete_01
│   ├── S_Footstep_Concrete_02
│   ├── S_Footstep_Concrete_03
│   └── S_Footstep_Concrete_04
├── Pitch Modulator (0.95 - 1.05)
└── Attenuation Settings (3D)
```

---

## 9. 実装優先順位

### Phase 1: 基本SFX
- [ ] プレイヤー足音（4バリエーション × 2表面）
- [ ] 撮影音（シャッター、フォーカス）
- [ ] 隠れる音
- [ ] UI効果音（検知ゲージ）

### Phase 2: アンビエンス
- [ ] ベースアンビエンス（各エリア）
- [ ] ランダムワンショットシステム
- [ ] リバーブ設定

### Phase 3: 演出音
- [ ] 監視システム音（カメラ、スポットライト）
- [ ] イベントスティンガー
- [ ] 緊張度レイヤー

### Phase 4: 調整
- [ ] 音量バランス調整
- [ ] 空間オーディオ調整
- [ ] パフォーマンス最適化

---

## Appendix: アセット命名規則

| タイプ | 接頭辞 | 例 |
|--------|--------|-----|
| Sound Wave | S_ | S_Footstep_Concrete_01 |
| Sound Cue | SC_ | SC_Footstep_Concrete |
| Sound Attenuation | SA_ | SA_Footstep_Default |
| Sound Mix | SM_ | SM_Night1_Default |
| Sound Class | SCL_ | SCL_SFX_Player |
| Reverb Effect | RE_ | RE_MainHall |
| Audio Volume | AV_ | AV_MainHall |

---

## Appendix: サウンドリスト

### 必要なサウンドファイル一覧

```
Player SFX (16 files)
├── S_Footstep_Concrete_01-04
├── S_Footstep_Metal_01-04
├── S_Footstep_Carpet_01-04
├── S_Camera_Shutter
├── S_Camera_Focus
├── S_Hide_Enter
└── S_Hide_Exit

Environment SFX (12 files)
├── S_Camera_Rotate
├── S_Spotlight_On
├── S_Spotlight_Move
├── S_Light_Flicker
├── S_Pipe_Creak
├── S_Electric_Spark
├── S_Door_Creak
├── S_Distant_Metal_01-04
└── S_System_Hum_Loop

Ambience (6 files)
├── S_Amb_Entrance
├── S_Amb_Corridor
├── S_Amb_MainHall
├── S_Amb_SmallRoom
├── S_Amb_DeepArea
└── S_Amb_TensionDrone

UI SFX (6 files)
├── S_UI_Detection_Up
├── S_UI_Detection_Warning
├── S_UI_Detection_Max
├── S_UI_Photo_Success
├── S_UI_Pause_On
└── S_UI_Pause_Off

Stingers (5 files)
├── S_Stinger_FirstLight
├── S_Stinger_Discovery
├── S_Stinger_Awakening
├── S_Stinger_Detected
└── S_Stinger_Dawn

Breath/Heartbeat (4 files)
├── S_Breath_Normal
├── S_Breath_Tense
├── S_Breath_Alert
└── S_Breath_Hiding

Total: ~49 audio files
```

---

**Document Status**: Draft
**Next Review**: 実装開始後
