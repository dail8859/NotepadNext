cask "notepadnext" do
  version "0.7"
  sha256 "50db724e698ec0756fbbf4f5cd17aaed48d9b089a333a9820603729da32ff166"

  url "https://github.com/dail8859/NotepadNext/releases/download/v#{version}/NotepadNext-v#{version}.dmg"
  name "notepad-next"
  desc "Cross-platform, reimplementation of Notepad++"
  homepage "https://github.com/dail8859/NotepadNext"

  livecheck do
    url :url
    strategy :github_latest
  end

  depends_on macos: ">= :big_sur"

  app "Notepadnext.app"

  # No zap stanza required
end
