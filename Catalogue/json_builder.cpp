#include "json_builder.h"

namespace json {

// ------------------- public -------------------

KeyItemCorrect Builder::Key(std::string key) {
  CheckBuildRoot();
  CheckErrorKey(key);

  if (index_chield == -1) {
    status_root_.save_key_ = key;
    status_root_.status_key = true;
  } else {
    status_build_chield[index_chield].save_key_ = key;
    status_build_chield[index_chield].status_key = true;
  }
  history_.push_back(Requests::KEY);
  return *this;
}

Interface Builder::Value(const Node::Value value) {
  CheckErrorValue();
  if (!status_root_.start_create_root) {
    root_ = CheckIndexVariant(value);
    status_root_.finish_create_root = true;
  } else if (status_root_.type_root == IndexVariant::IN_DICT &&
             index_chield == -1) {
    InsertKeyAndValue(value, const_cast<Dict&>(root_.AsMap()),
                      status_root_.save_key_);
    status_root_.status_key = false;
  } else if (status_root_.type_root == IndexVariant::IN_DICT) {
    if (status_build_chield.back().type_root == IndexVariant::IN_DICT) {
      InsertKeyAndValue(
          value, const_cast<Dict&>(nods_form_chield_[index_chield].AsMap()),
          status_build_chield[index_chield].save_key_);
      status_build_chield[index_chield].status_key = false;
    } else if (status_build_chield.back().type_root == IndexVariant::IN_ARRAY) {
      const_cast<Array&>(nods_form_chield_[index_chield].AsArray())
          .push_back(CheckIndexVariant(value));
    }

  } else if (status_root_.type_root == IndexVariant::IN_ARRAY &&
             index_chield == -1) {
    const_cast<Array&>(root_.AsArray()).push_back(CheckIndexVariant(value));
  } else if (status_root_.type_root == IndexVariant::IN_ARRAY) {
    if (status_build_chield.back().type_root == IndexVariant::IN_ARRAY) {
      const_cast<Array&>(nods_form_chield_[index_chield].AsArray())
          .push_back(CheckIndexVariant(value));
    } else if (status_build_chield.back().type_root == IndexVariant::IN_DICT) {
      InsertKeyAndValue(
          value, const_cast<Dict&>(nods_form_chield_[index_chield].AsMap()),
          status_build_chield[index_chield].save_key_);
      status_build_chield[index_chield].status_key = false;
    }
  }
  history_.push_back(Requests::VALUE);
  return *this;
}

ValueItemCorrect Builder::StartDict() {
  CheckBuildRoot();
  CheckErrorStartDict();
  if (!status_root_.start_create_root) {
    Node::Value val;
    val.emplace<Dict>();
    root_ = CheckIndexVariant(val);
    status_root_.start_create_root = true;
    for_check_tipe_form_.push_back(IndexVariant::IN_DICT);
    status_root_.type_root = IndexVariant::IN_DICT;
  } else {
    if (status_build_chield.empty()) {
      Node::Value val;
      val.emplace<Dict>();
      nods_form_chield_.push_back(CheckIndexVariant(val));
      StatusBuild for_chield;
      for_chield.type_root = IndexVariant::IN_DICT;
      status_build_chield.push_back(for_chield);
      index_chield += 1;
    } else if (status_build_chield.back().type_root == IndexVariant::IN_ARRAY) {
      Node::Value val;
      val.emplace<Dict>();
      StatusBuild for_chield;
      for_chield.type_root = IndexVariant::IN_DICT;
      for_chield.index_add = nods_form_chield_.size() - 1;
      nods_form_chield_.push_back(CheckIndexVariant(val));
      status_build_chield.push_back(for_chield);
      index_chield += 1;
    }
  }
  history_.push_back(Requests::START_DICT);
  return *this;
}

Interface Builder::EndDict() {
  CheckBuildRoot();
  CheckErrorEndDict();
  if (index_chield > -1) {
    if (status_build_chield[index_chield].index_add == -1) {
      if (status_build_chield[index_chield].status_key) {
        const_cast<Dict&>(
            nods_form_chield_[index_chield]
                .AsMap())[status_build_chield[index_chield].save_key_];
      }
      if (status_root_.type_root == IndexVariant::IN_DICT) {
        const_cast<Dict&>(root_.AsMap())[status_root_.save_key_] =
            nods_form_chield_.back();
        status_root_.save_key_.clear();
        status_root_.status_key = false;
        EraceChield();
      } else if (status_root_.type_root == IndexVariant::IN_ARRAY) {
        const_cast<Array&>(root_.AsArray()).push_back(nods_form_chield_.back());
        EraceChield();
      }
    } else {
      if (status_build_chield[index_chield].status_key) {
        const_cast<Dict&>(
            nods_form_chield_[index_chield]
                .AsMap())[status_build_chield[index_chield].save_key_];
      }
      int index = status_build_chield[index_chield].index_add;
      if (status_build_chield[index].type_root == IndexVariant::IN_DICT) {
        const_cast<Dict&>(
            nods_form_chield_[index].AsMap())[status_root_.save_key_] =
            nods_form_chield_.back();
        status_root_.save_key_.clear();
        status_root_.status_key = false;
        EraceChield();
      } else if (status_build_chield[index].type_root ==
                 IndexVariant::IN_ARRAY) {
        const_cast<Array&>(nods_form_chield_[index].AsArray())
            .push_back(nods_form_chield_.back());
        EraceChield();
      }
    }
  } else {
    status_root_.finish_create_root = true;
  }
  history_.push_back(Requests::END_DICT);
  return *this;
}

ArrayItemCorrect Builder::StartArray() {
  CheckBuildRoot();
  CheckErrorStartArray();
  if (!status_root_.start_create_root) {
    Node::Value val;
    val.emplace<Array>();
    root_ = CheckIndexVariant(val);
    status_root_.start_create_root = true;
    for_check_tipe_form_.push_back(IndexVariant::IN_ARRAY);
    status_root_.type_root = IndexVariant::IN_ARRAY;
  } else {
    Node::Value val;
    val.emplace<Array>();
    nods_form_chield_.push_back(CheckIndexVariant(val));
    StatusBuild for_chield;
    for_chield.type_root = IndexVariant::IN_ARRAY;
    status_build_chield.push_back(for_chield);
    index_chield += 1;
  }
  history_.push_back(Requests::START_ARRAY);
  return *this;
}

Interface Builder::EndArray() {
  CheckBuildRoot();
  CheckErrorEndArray();
  if (index_chield > -1) {
    if (status_root_.type_root == IndexVariant::IN_DICT) {
      const_cast<Dict&>(root_.AsMap())[status_root_.save_key_] =
          nods_form_chield_.back();
      status_root_.save_key_.clear();
      status_root_.status_key = false;
      EraceChield();
    } else if (status_root_.type_root == IndexVariant::IN_ARRAY) {
      const_cast<Array&>(root_.AsArray()).push_back(nods_form_chield_.back());
      EraceChield();
    }
  } else {
    status_root_.finish_create_root = true;
  }
  history_.push_back(Requests::END_ARRAY);
  return *this;
}

Node Builder::Build() {
  if (!status_root_.finish_create_root) {
    PrintError("Error finish create root");
  }
  return root_;
}

// ------------------- private -------------------

Node Builder::ValueNull(const Node::Value value) {
  Node buffer(std::get<std::nullptr_t>(value));
  return buffer;
}

Node Builder::ValueArray(const Node::Value value) {
  Node buffer(std::get<Array>(value));
  return buffer;
}

Node Builder::ValueDict(const Node::Value value) {
  Node buffer(std::get<Dict>(value));
  return buffer;
}

Node Builder::ValueBool(const Node::Value value) {
  Node buffer(std::get<bool>(value));
  return buffer;
}

Node Builder::ValueInt(const Node::Value value) {
  Node buffer(std::get<int>(value));
  return buffer;
}

Node Builder::ValueDbl(const Node::Value value) {
  Node buffer(std::get<double>(value));
  return buffer;
}

Node Builder::ValueString(const Node::Value value) {
  Node buffer(std::get<std::string>(value));
  return buffer;
}

Node Builder::CheckIndexVariant(const Node::Value value) {
  size_t index = value.index();
  Node ans;
  switch (index) {
    case IN_NULL:
      ans = ValueNull(value);
      break;
    case IN_ARRAY:
      ans = ValueArray(value);
      break;
    case IN_DICT:
      ans = ValueDict(value);
      break;
    case IN_BOOL:
      ans = ValueBool(value);
      break;
    case IN_INT:
      ans = ValueInt(value);
      break;
    case IN_DBL:
      ans = ValueDbl(value);
      break;
    case IN_STR:
      ans = ValueString(value);
      break;

    default:
      PrintError("Error type in variant");
      break;
  }
  return ans;
}

void Builder::PrintError(std::string text_error) {
  throw std::logic_error(text_error);
}

bool Builder::CheckBuildRoot() {
  if (status_root_.finish_create_root) {
    PrintError("Object creation completed");
  }
  return status_root_.finish_create_root;
}

void Builder::EraceChield() {
  if (!nods_form_chield_.empty() && !status_build_chield.empty()) {
    nods_form_chield_.erase(nods_form_chield_.end() - 1);
    status_build_chield.erase(status_build_chield.end() - 1);
    --index_chield;
  } else {
    PrintError("Erace empty chield");
  }
}

bool Builder::CheckErrorStartDict() {
  if (history_.back() == Requests::CREATE) {
    return true;
  }
  if (index_chield == -1) {
    if (status_root_.type_root == IndexVariant::IN_DICT &&
        history_.back() == Requests::KEY) {
      return true;
    }
    if (status_root_.type_root == IndexVariant::IN_ARRAY &&
        history_.back() == Requests::VALUE) {
      return true;
    }
    if (status_root_.type_root == IndexVariant::IN_ARRAY &&
        history_.back() == Requests::END_DICT) {
      return true;
    }
    if (status_root_.type_root == IndexVariant::IN_ARRAY &&
        history_.back() == Requests::END_ARRAY) {
      return true;
    }
  } else {
    if (status_build_chield[index_chield].type_root == IndexVariant::IN_DICT &&
        history_.back() == Requests::KEY) {
      return true;
    }
    if (status_build_chield[index_chield].type_root == IndexVariant::IN_ARRAY &&
        history_.back() == Requests::VALUE) {
      return true;
    }
    if (status_build_chield[index_chield].type_root == IndexVariant::IN_ARRAY &&
        history_.back() == Requests::END_DICT) {
      return true;
    }
    if (status_build_chield[index_chield].type_root == IndexVariant::IN_ARRAY &&
        history_.back() == Requests::END_ARRAY) {
      return true;
    }
  }
  if (history_.back() == Requests::START_ARRAY) {
    return true;
  }
  PrintError("Incorrect call StartDict");
  return false;
}
bool Builder::CheckErrorStartArray() {
  if (history_.back() == Requests::CREATE) {
    return true;
  }
  if (index_chield == -1) {
    if (status_root_.type_root == IndexVariant::IN_DICT &&
        history_.back() == Requests::KEY) {
      return true;
    }
    if (status_root_.type_root == IndexVariant::IN_ARRAY &&
        history_.back() == Requests::VALUE) {
      return true;
    }
    if (status_root_.type_root == IndexVariant::IN_ARRAY &&
        history_.back() == Requests::END_DICT) {
      return true;
    }
    if (status_root_.type_root == IndexVariant::IN_ARRAY &&
        history_.back() == Requests::END_ARRAY) {
      return true;
    }
  } else {
    if (status_build_chield[index_chield].type_root == IndexVariant::IN_DICT &&
        history_.back() == Requests::KEY) {
      return true;
    }
    if (status_build_chield[index_chield].type_root == IndexVariant::IN_ARRAY &&
        history_.back() == Requests::VALUE) {
      return true;
    }
    if (status_build_chield[index_chield].type_root == IndexVariant::IN_ARRAY &&
        history_.back() == Requests::END_DICT) {
      return true;
    }
    if (status_build_chield[index_chield].type_root == IndexVariant::IN_ARRAY &&
        history_.back() == Requests::END_ARRAY) {
      return true;
    }
  }
  if (history_.back() == Requests::START_ARRAY) {
    return true;
  }
  // if (history_.back() == Requests::CREATE || history_.back() ==
  // Requests::VALUE ||
  //     history_.back() == Requests::START_ARRAY || history_.back() ==
  //     Requests::KEY) { return true;
  // }
  PrintError("Incorrect call StartArray");
  return false;
}

bool Builder::CheckErrorEndDict() {
  if (index_chield == -1) {
    if (status_root_.type_root != IndexVariant::IN_DICT) {
      PrintError("Incorrect call EndDict");
    }
  } else {
    if (status_build_chield.back().type_root != IndexVariant::IN_DICT) {
      PrintError("Incorrect call EndDict");
    }
  }
  return true;
}

bool Builder::CheckErrorEndArray() {
  if (index_chield == -1) {
    if (status_root_.type_root != IndexVariant::IN_ARRAY) {
      PrintError("Incorrect call EndDict");
    }
  } else {
    if (status_build_chield.back().type_root != IndexVariant::IN_ARRAY) {
      PrintError("Incorrect call EndDict");
    }
  }
  return true;
}

// ---------- Value ----------
bool Builder::CheckErrorValue() {
  if (history_.back() == Requests::CREATE) {
    return true;
  }
  if (index_chield == -1) {
    if (status_root_.type_root == IndexVariant::IN_DICT &&
        history_.back() == Requests::KEY) {
      return true;
    }
    if (status_root_.type_root == IndexVariant::IN_ARRAY &&
        history_.back() == Requests::VALUE) {
      return true;
    }
    if (status_root_.type_root == IndexVariant::IN_ARRAY &&
        history_.back() == Requests::END_DICT) {
      return true;
    }
    if (status_root_.type_root == IndexVariant::IN_ARRAY &&
        history_.back() == Requests::END_ARRAY) {
      return true;
    }
  } else {
    if (status_build_chield[index_chield].type_root == IndexVariant::IN_DICT &&
        history_.back() == Requests::KEY) {
      return true;
    }
    if (status_build_chield[index_chield].type_root == IndexVariant::IN_ARRAY &&
        history_.back() == Requests::VALUE) {
      return true;
    }
    if (status_build_chield[index_chield].type_root == IndexVariant::IN_ARRAY &&
        history_.back() == Requests::END_DICT) {
      return true;
    }
    if (status_build_chield[index_chield].type_root == IndexVariant::IN_ARRAY &&
        history_.back() == Requests::END_ARRAY) {
      return true;
    }
  }
  if (history_.back() == Requests::START_ARRAY) {
    return true;
  }

  PrintError("Incorrect call Value");
  return false;
}

void Builder::InsertKeyAndValue(const Node::Value& value, Dict& src,
                                std::string& key) {
  Node witn_value = CheckIndexVariant(value);
  src.insert({key, witn_value});
  key.clear();
}
// ---------- Value ----------
// ---------- Key ----------
bool Builder::CheckErrorKey(std::string& key) {
  if (index_chield == -1) {
    if (status_root_.type_root != IndexVariant::IN_DICT) {
      PrintError("Initialization key outside dictionary");
    }
  } else {
    if (status_build_chield.back().type_root != IndexVariant::IN_DICT) {
      PrintError("Initialization key outside dictionary");
    }
  }
  if (history_.back() == Requests::KEY) {
    PrintError("re-adding the key: " + key + " ");
  }
  return true;
}
// ---------- Key ----------

// ---------- Interface ----------

KeyItemCorrect Interface::Key(std::string key) { return builder_.Key(key); }
Interface Interface::Value(const Node::Value value) {
  return builder_.Value(value);
}
ValueItemCorrect Interface::StartDict() { return builder_.StartDict(); }
ArrayItemCorrect Interface::StartArray() { return builder_.StartArray(); }
Interface Interface::EndDict() { return builder_.EndDict(); }
Interface Interface::EndArray() { return builder_.EndArray(); }
Node Interface::Build() { return builder_.Build(); }

// ---------- Interface ----------

// ---------- KeyItemCorrect ----------

ValueItemCorrect KeyItemCorrect::Value(const Node::Value value) {
  return builder_.Value(value).GetBuilder();
}
ValueItemCorrect KeyItemCorrect::StartDict() { return builder_.StartDict(); }
Interface KeyItemCorrect::StartArray() { return builder_.StartArray(); }

// ---------- KeyItemCorrect ----------

// ---------- ValueItemCorrect ----------

KeyItemCorrect ValueItemCorrect::Key(std::string key) {
  return builder_.Key(key);
}
Interface ValueItemCorrect::EndDict() { return builder_.EndDict(); }

// ---------- ValueItemCorrect ----------

// ---------- ArrayItemCorrect ----------

ArrayItemCorrect ArrayItemCorrect::Value(const Node::Value value) {
  return builder_.Value(value).GetBuilder();
}
ValueItemCorrect ArrayItemCorrect::StartDict() { return builder_.StartDict(); }
ArrayItemCorrect ArrayItemCorrect::StartArray() {
  return builder_.StartArray();
}
Interface ArrayItemCorrect::EndArray() { return builder_.EndArray(); }

// ---------- ArrayItemCorrect ----------

}  // namespace json